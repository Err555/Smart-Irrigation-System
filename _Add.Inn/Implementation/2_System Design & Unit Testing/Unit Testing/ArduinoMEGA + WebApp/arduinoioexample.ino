#include "SPI.h"
#include "Phpoc.h"

#define CMD_ARDUINO_SET_MODE	0x01
#define CMD_ARDUINO_SET_STATE	0x02
#define CMD_ARDUINO_GET_MODE	0x03
#define CMD_ARDUINO_GET_STATE	0x04

#define CMD_WEB_UPDATE_STATE	0x10
#define CMD_WEB_UPDATE_MODE		0x11

#define MODE_FLAG_IO	0x00
#define MODE_FLAG_COMM	0x10 // communication

#define MODE_INPUT		0x00
#define MODE_OUTPUT		0x01
#define MODE_ANALOG		0x03
#define MODE_PWM		0x04
#define MODE_SPI		0x10
#define MODE_I2C		0x11
#define MODE_UART		0x12


PhpocServer server(80);

/* 14 digital pin and 6 analog pin */
int pinModes[20] = {MODE_INPUT, MODE_INPUT, MODE_INPUT, MODE_INPUT, MODE_INPUT, MODE_INPUT, MODE_INPUT, MODE_INPUT, MODE_INPUT, MODE_INPUT, MODE_INPUT, MODE_INPUT, MODE_INPUT, MODE_INPUT, MODE_ANALOG, MODE_ANALOG, MODE_ANALOG, MODE_ANALOG, MODE_ANALOG, MODE_ANALOG};
int pinStates[20];

unsigned long updateInterval;
unsigned long lastUpdateTime;
char wbuf[20];

void sendToWeb(int pin, int cmd, float state) ;

void setup() {
    Serial.begin(9600);
    while(!Serial)
        ;

    Phpoc.begin(PF_LOG_SPI | PF_LOG_NET);

    server.beginWebSocket("arduino_io");

    Serial.print("WebSocket server address : ");
    Serial.println(Phpoc.localIP());
	
	pinModes[0] = MODE_UART;
	pinModes[1] = MODE_UART;
	pinModes[10] = MODE_SPI;
	pinModes[11] = MODE_SPI;
	pinModes[12] = MODE_SPI;
	pinModes[13] = MODE_SPI;
	
	for(int pin = 0; pin < 20; pin++) {
		if(pinModes[pin] == MODE_INPUT)
			pinMode(pin, INPUT_PULLUP);
		
		if(pinModes[pin] == MODE_ANALOG)
			pinStates[pin] = analogRead(pin);
		else
		if(pinModes[pin] == MODE_INPUT || pinModes[pin] == MODE_OUTPUT )
			pinStates[pin] = digitalRead(pin);
	}
	
	updateInterval = 100; // 100 milllisecond
    lastUpdateTime = 0;
}

void loop() {
    // when the client sends the first byte, say hello:
    PhpocClient client = server.available();
    
    if (client) {
		String data = client.readLine();
		
		/* example of data: A5:1:0 in order pin:command:value */
		
		if(data) {
            int firstConlon = data.indexOf(':');
			int secondConlon = data.indexOf(':', firstConlon + 1);
			
			char group = data.charAt(0); /* A or D */
            int pin = data.substring(1, firstConlon).toInt();
			int cmd = data.substring(firstConlon + 1, secondConlon).toInt();
			int value = data.substring(secondConlon + 1).toInt();
			
			/* in Arduino Uno, pin A0 to A5 is mapped from pin 14 to 19 */
			if(group == 'A')
				pin += 14;
			
            if(cmd == CMD_ARDUINO_SET_MODE) {
				if(group == 'A' && value == MODE_ANALOG)
					pinMode(pin, MODE_INPUT);
				else
				if(value == MODE_INPUT)
					pinMode(pin, INPUT_PULLUP);
				else
					pinMode(pin, value);
				
				pinModes[pin] = value;
				sendToWeb(pin, CMD_WEB_UPDATE_MODE, pinModes[pin]);
			}
			else
			if(cmd == CMD_ARDUINO_SET_STATE)
			{
				if(pinModes[pin] == OUTPUT)
					digitalWrite(pin, value);
				else
				if(pinModes[pin] == MODE_PWM)
					analogWrite(pin, value);
				
				pinStates[pin] = value;
				//sendToWeb(pin, CMD_WEB_UPDATE_STATE, pinStates[pin]);
			}
			else
			if(cmd == CMD_ARDUINO_GET_MODE)
				sendToWeb(pin, CMD_WEB_UPDATE_MODE, pinModes[pin]);
			else
			if(cmd == CMD_ARDUINO_GET_STATE)
			{
				if(pinModes[pin] & MODE_FLAG_COMM)
				{
					if(pin == 0)
						sendToWeb(pin, CMD_WEB_UPDATE_STATE, String("RX"));
					else
					if(pin == 1)
						sendToWeb(pin, CMD_WEB_UPDATE_STATE, String("TX"));
					else
					if(pin == 10)
						sendToWeb(pin, CMD_WEB_UPDATE_STATE, String("MOSI"));
					else
					if(pin == 11)
						sendToWeb(pin, CMD_WEB_UPDATE_STATE, String("MISO"));
					else
					if(pin == 12)
						sendToWeb(pin, CMD_WEB_UPDATE_STATE, String("SCK"));
					else
					if(pin == 13)
						sendToWeb(pin, CMD_WEB_UPDATE_STATE, String("SS"));
				}
				else
					sendToWeb(pin, CMD_WEB_UPDATE_STATE, pinStates[pin]);
			}
			else
				Serial.println("unknown command");
		}
    }
	
	if ((millis() - lastUpdateTime) > updateInterval) {
		for(int pin = 0; pin < 20; pin++) {
			
			int newState;
			
			if(pinModes[pin] == MODE_ANALOG)
			{
				newState = analogRead(pin);
				if(abs(newState - pinStates[pin]) > 5) {
					sendToWeb(pin, CMD_WEB_UPDATE_STATE, newState);
					pinStates[pin] = newState;
				}
			}
			else
			if(pinModes[pin] == MODE_INPUT || pinModes[pin] == MODE_OUTPUT )
			{
				newState = digitalRead(pin);
				if(newState != pinStates[pin]) {
					sendToWeb(pin, CMD_WEB_UPDATE_STATE, newState);
					pinStates[pin] = newState;
				}
			}
		}
		
		lastUpdateTime = millis();
	}
}

void sendToWeb(int pin, int cmd, float state) {
	String txtMsg;
	
	if(pin < 14)
		txtMsg =  String("D") + String(pin) + ":"+ String(cmd) + ":" + String(state) + "\r\n";  
	else
		txtMsg =  String("A") + String(pin - 14) + ":"+ String(cmd) + ":" + String(state) + "\r\n";
	
	txtMsg.toCharArray(wbuf, txtMsg.length() + 1);
	server.write(wbuf, txtMsg.length());
}
void sendToWeb(int pin, int cmd, String state) {
	String txtMsg;
	
	if(pin < 14)
		txtMsg =  String("D") + String(pin) + ":"+ String(cmd) + ":" + state + "\r\n";  
	else
		txtMsg =  String("A") + String(pin - 14) + ":"+ String(cmd) + ":" + state + "\r\n";
	
	txtMsg.toCharArray(wbuf, txtMsg.length() + 1);
	server.write(wbuf, txtMsg.length());
}