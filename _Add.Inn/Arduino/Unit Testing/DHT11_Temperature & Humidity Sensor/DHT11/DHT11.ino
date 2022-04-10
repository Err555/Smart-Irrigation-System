#include "DHT.h"
#include <LiquidCrystal.h>
#define DHTPIN A0
#define DHTTYPE DHT11


DHT dht(DHTPIN, DHTTYPE);

int ph_analog = A1;
int ph_analog_val;

int sensorPin = A2; 
int sensorValue;
int limit = 300;

const int buzzerPin = 9;
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
 Serial.begin(9600);
  dht.begin(); 
  // PH Sensor Begins
pinMode(ph_analog, INPUT);
// PH Sensor Ends}

 // Moisture Sensor Begins
pinMode(13, OUTPUT);
// Moisture Sensor Ends

// Buzzer Begins
pinMode(buzzerPin, OUTPUT);
// Buzzer Ends
//LCD start
  lcd.begin(16, 2);
  lcd.println("Smart Irrigation System");
  //Lcd Ends
  // GSM start
    Serial.begin(9600);  //Baud rate of the GSM/GPRS Module 
  delay(1000);                  
  Serial.print("AT+CMGF=1\r");    
  delay(1000);
  Serial.print("AT+CMGS=\"+91xxxxxxxxxx\"\r");    //Phone number you want to send the sms
  delay(1000);
  Serial.print("Hello Maxphi\r");   //Text message you want to send
  delay(1000);
  Serial.write(0x1A); //sends ctrl+z end of message 
  delay(1000); 
//GSM Ends
}
void loop() {
  // wait a few seconds between measurements.
  delay(2000);

  // read humidity
  float humi  = dht.readHumidity();
  // read temperature as Celsius
  float tempC = dht.readTemperature();
  // read temperature as Fahrenheit
  float tempF = dht.readTemperature(true);

  // check if any reads failed
  if (isnan(humi) || isnan(tempC) || isnan(tempF)) {
    Serial.println("Failed to read from Temperature & Humidity Sensor!");
  } else {
    Serial.print("Humidity: ");
    Serial.print(humi);
    Serial.print("%");

    Serial.print("  |  "); 

    Serial.print("Temperature: ");
    Serial.print(tempC);
    Serial.print("°C ~ ");
    Serial.print(tempF);
    Serial.println("°F");

  // PH Sensor Begins
  ph_analog_val= analogRead(ph_analog);
Serial.print("PH Value - ");
Serial.println(ph_analog_val);
//PH Sensor Ends
// Moisture Sensor Begins
sensorValue = analogRead(sensorPin); 

Serial.print("Moisture Value - ");
Serial.println(sensorValue);
Serial.println();

if (sensorValue<limit) {
digitalWrite(13, HIGH); 
}
else {
digitalWrite(13, LOW);
 }
delay(1000); 
}
  // Moisture Sensor Ends
  // Buzzer Begins
   tone(buzzerPin, 50);
  delay(50);
  noTone(buzzerPin);
  delay(100);
  // Buzzer Ends
    // setting the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 1);
  // print the number of seconds since reset:
  lcd.print(millis() / 1000);
  //GSM Starts
  
  }
