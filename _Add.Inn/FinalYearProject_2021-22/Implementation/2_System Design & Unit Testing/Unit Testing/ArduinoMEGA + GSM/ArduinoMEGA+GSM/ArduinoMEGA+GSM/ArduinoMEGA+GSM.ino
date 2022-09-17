 #include <SoftwareSerial.h>
SoftwareSerial mySerial(19, 18);
void setup() 
{

  mySerial.begin(9600);   // Setting the baud rate of GSM Module
  Serial.begin(9600); 
}
void loop() {
  SendMessage();
}
  void SendMessage()
{
   mySerial.println("Setting the GSM in text mode");
   mySerial.println("AT+CMGF=1\r");
   delay(2000);
   mySerial.println("Sending SMS to the desired phone number!");
   mySerial.println("AT+CMGS=\"+255783889987\"\r");
   delay(2000);

   mySerial.println("Hello there, I am SIM800C");  
   delay(200);
   mySerial.println((char)26);              
   delay(2000);
}
