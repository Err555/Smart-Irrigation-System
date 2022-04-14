#include "DHT.h"
#include <LiquidCrystal.h>
#include <Wire.h>
#include<EEPROM.h>
#include <RTClib.h>
#include <LiquidCrystal.h>
#include <Keypad.h>
#include <SoftwareSerial.h>

#define DHTPIN 10
#define DHTTYPE DHT11

RTC_DS1307 RTC;
LiquidCrystal lcd(12, 11, 6, 5, 4, 3);
DHT dht(DHTPIN, DHTTYPE);

int ph_analog = A1;
int ph_analog_val;

int moistPin = A0; 
int moistValue;
int limit = 300;

const int buzzerPin = 9;

// ##################################################################################################################################
void setup() {

  Serial.begin(9600);   // Setting the baud rate of GSM Module  
  Serial.begin(9600);    // Setting the baud rate of Serial Monitor (Arduino)
 /* pinMode( relay, OUTPUT ); 
  digitalWrite( relay, LOW );
  */ 
  Serial.println( "AT+CMGF=1" ); 
  delay(200);
 // Wire.begin();
  RTC.begin();
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcd.print("Smart Irrigation");  
  lcd.setCursor(0,1);
  lcd.print("MOISTURE DETECT ");
  delay(3000);
 if(!RTC.isrunning())
 {
 RTC.adjust(DateTime(__DATE__,__TIME__));
 } 

digitalWrite(12, LOW);
lcd.setCursor(0,0);
lcd.print("ENTER MOBILE NO.");
lcd.setCursor(0,1);
lcd.print(" YES*       NO# ");
DateTime now = RTC.now();
prevTime= now.minute();
//lcd.print("                ");
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
moistValue = analogRead(moistPin); 

Serial.print("Moisture Value - ");
Serial.println(moistValue);
Serial.println();

if (moistValue<limit) {
digitalWrite(13, HIGH); 
}
else {
digitalWrite(13, LOW);
 }
delay(1000); 
}
// char key = keypad.getKey();
                 while(pos==0){
//                  char key = keypad.getKey();
//                 if(key=='*' && pos==0){
                  lcd.setCursor(0,0);
                  lcd.print("ENTER MOBILE NO.");
                lcd.setCursor(0,1);
                lcd.print("                ");
//                 saveNUM();
               }
//               if(key=='#' && pos==0){
                 //sendSMS();
                lcd.clear();
                pos=2;
            //    break; 
               }   
 moist=analogRead(A2);
 if(moist>=950){
  a=0;
  // while(1){
  lowAlertSMS();
  lcd.setCursor(0,0); 
  lcd.print(" SOIL MOISTURE-  ");
  lcd.setCursor(0,1); 
  lcd.print("  LOW DETECTED  ");
  delay(1000);
  while(1){
char SerialInByte;
moist=analogRead(A0);
    if(Serial.available())
    {       
        SerialInByte = (unsigned char)Serial.read();
       delay(5);
        if( SerialInByte == 13 ){
           ProcessGprsMsg();
           Serial.println( "*** SMS Received ***" );
         }
         if( SerialInByte == 10 ){
         }
         else {
           msg += String(SerialInByte);
         }
     }   
if(a==1 ||moist <=950){
  digitalWrite(  relay, LOW );
  lcd.setCursor(0,0);
  lcd.print("  MOTOR IS IN   ");
  lcd.setCursor(0,1);
  lcd.print(" OFF CONDITION  ");
  lcd.clear();
  break;
  }
  }
 }
// if(key=='1'){
 moistureDETECT();
 }
// if(key=='2'){
 detectNUM();
 }
//if(key=='0'){
 lcd.setCursor(0,0);
 lcd.print("ENTER MOBILE NO.");
 lcd.setCursor(0,1);
 lcd.print("                ");
 saveNUM();
 }
// if(key=='#'){
  lcd.clear();
  timerSET();
 }
// if(key=='3'){
  lcd.clear();
  value = EEPROM.read(11);
  lcd.setCursor(0,0); 
  lcd.print("  TIME SET FOR  ");
  lcd.setCursor(0,1); 
  lcd.print("  EVERY   HOUR   ");
  lcd.setCursor(8,1);
  lcd.print(value);
  delay(3000);
  delay(3000);
 }
// if(key=='*'){
  lcd.clear();
  for(int i=1;i<41;i++){
 Serial.print( "AT+CMGD=" );
 Serial.println(i);
 lcd.setCursor(0,0);
 lcd.print(" DELETING SIM   ");
 lcd.setCursor(5,1);
 lcd.print("MSGS");
 lcd.setCursor(10,1);
 lcd.print(i);
 delay(500);
  }
  }
 delay(1000);
  lcd.setCursor(8,0); 
  lcd.print(" ");
  lcd.setCursor(10,1); 
  lcd.print(" ");
  
 DHT.read11(dht_dpin);
 DateTime now = RTC.now();
 lcd.setCursor(0,0);
 printDigits2(HOUR=now.hour()); 
 lcd.print(":");
 printDigits2(MINUT=now.minute());
 lcd.print(":");
 newTime = now.minute();
 printDigits2(SECOND=now.second());
 lcd.setCursor(0,1);
 lcd.print(now.day(),DEC);
 lcd.print("/");
 lcd.print(now.month(),DEC);
 lcd.print("/");
 lcd.print(now.year(),DEC);
 lcd.setCursor(0,1);
 lcd.print(now.day(),DEC);
 lcd.print("/");
 lcd.print(now.month(),DEC);
 lcd.print("/");
 lcd.print(now.year(),DEC);
 lcd.setCursor(9,0); 
 lcd.print("H:");
 lcd.print(DHT.humidity);
 lcd.setCursor(15,0); 
 lcd.print("%");
 lcd.setCursor(11,1);
 lcd.print("T:");
 lcd.print(DHT.temperature);
 lcd.print((char)223);
 lcd.setCursor(15,1); 
 lcd.print("C");
 matchTIM();
}

//###################################################################################################################
void saveNUM(){  
    
    while(1){
//    char key = keypad.getKey();
//  if (key){
//    Serial.println(key);
  //  phno[i]=key;
    num[i]= phno[i]-48;
    lcd.setCursor(13,1);
    lcd.print("    ");
    lcd.setCursor(i+3,1);
    lcd.print(num[i]);
    count++;
    i++;
    delay(500);
//   if(key=='#' && count==11){
    for(int j=0;j<11;j++){
    lcd.setCursor(0,0);
    lcd.print("***** WAIT *****");
    EEPROM.write(j, num[j]);
    delay(500);
    }
   lcd.setCursor(0,0);
   lcd.print("  NUMBER SAVED  ");
   lcd.setCursor(0,1);
   lcd.print("***************");
   lcd.clear();
   pos=1;
   
   break;

    } 
  }
}
}
// ####################################################################################################################################
void printDigits2(int digits)  //this void function is really useful; it adds a "0" to the beginning of the number, so that 5 minutes is displayed as "00:05:00", rather than "00:5 :00"

{
  if(digits < 10) 
  {
    lcd.print("0");
    lcd.print(digits);
  }
  else
  {
    lcd.print(digits);
  }
}
//####################################################################################################################################
void printDigits3(int digits)  //this void function is really useful; it adds a "0" to the beginning of the number, so that 5 minutes is displayed as "00:05:00", rather than "00:5 :00"

{
  if(digits < 1000) 
  {
    lcd.print(" ");
    lcd.print(digits);
  }
  else
  {
    lcd.print(digits);
  }
}
// ####################################################################################################################################
void moistureDETECT(){

 lcd.clear();
  lcd.setCursor(0,0); 
  lcd.print("Moisture Content");
 while(1){
//        char key = keypad.getKey();
        moist=analogRead(A0);
        lcd.setCursor(6,1); 
        printDigits3(moist);
//       if(key=='*'){
        lcd.clear();
        break;
        }
  }  
}
// ###################################################################################################################################
void detectNUM(){

  lcd.clear();
  lcd.setCursor(0,0); 
  lcd.print("   OWNER's NO.   ");
for(int j=0;j<11;j++){
   value = EEPROM.read(j);
   lcd.setCursor(j+3,1);
   lcd.print(value);
   lcd.setCursor(13,1);
   lcd.print("    ");
   delay(300);
}

 while(1){

// char key = keypad.getKey();
//  if(key=='*'){
  lcd.clear();
  break;
  }
 }
}
// ###################################################################################################################################
void timerSET(){

  int k=0;
  lcd.setCursor(0,0); 
  lcd.print("SET TIME FOR: ");

while(1){

// char key = keypad.getKey();
lcd.setCursor(13,0); 
while(pos==2){
   DateTime now = RTC.now();
// char key = keypad.getKey();
//  if(key){
//  lcd.print(key);
//  delayTIM=key-48;
  prevTime=now.minute();
  EEPROM.write(12, prevTime);
  delay(500);
  pos=3;  
  break;
    }
}

// if(pos==3 && key=='#'){

   EEPROM.write(11, delayTIM);
   lcd.setCursor(0,0);
   lcd.print("DELAY FOR   HOUR");
   lcd.setCursor(10,0);
   lcd.print(delayTIM);
   delay(3000);
   lcd.clear();
   pos=2;   
   break;
 }
 }  
}

void matchTIM(){

  DateTime now = RTC.now();
  hoursE=EEPROM.read(11);
if(newTime== prevTime + hoursE){
   value = EEPROM.read(11);
   prevTime=now.minute();
   while(1){
//   char key = keypad.getKey();
   lcd.setCursor(0,0);
   lcd.print("  TIME MATCHED  ");
   lcd.setCursor(0,1);
   lcd.print("  FOR    HOUR    "); 
   lcd.setCursor(7,1);
   lcd.print(value);
   moistureAlertSMS();
   lcd.clear();
   delay(1000);
   
//   if(key=='#'){
    break;
   }
 }
}
}
// ###################################################################################################################################
void lowAlertSMS(){

  delay(100);
  Serial.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1000 milli seconds or 1 second
  //mySerial.println("AT+CMGS="+255783889987"r"); // Replace x with mobile number
  Serial.print("AT+CMGS="+255745100699"r");
  for(int j=0;j<10;j++){
   value = EEPROM.read(j);
  Serial.print(value);
   delay(3);
  }
  Serial.println("r");
  delay(1000);
  Serial.println("Alert !!! Low moisture level Detected");// The SMS text you want to send
  delay(100);
  Serial.println((char)26);// ASCII code of CTRL+Z
  delay(1000);
}
// ###################################################################################################################################
void moistureAlertSMS(){

   moist=analogRead(A2);
  delay(100);
  Serial.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1000 milli seconds or 1 second
  //mySerial.println("AT+CMGS="+917357187588"r"); // Replace x with mobile number
  Serial.print("AT+CMGS="+255783889987"r");
  for(int j=0;j<10;j++){
   value = EEPROM.read(j);
   Serial.print(value);
   delay(3);
  }
 Serial.println("r");
  delay(1000);
  Serial.print("Soil Moisture is:");// The SMS text you want to send
  delay(100);
 Serial.println(moist);// The SMS text you want to send
  delay(100);
  Serial.println((char)26);// ASCII code of CTRL+Z
  delay(1000);
}
// ####################################################################################################################################
void ProcessSms(String sms){

 if(sms.indexOf("ONMOTOR") >= 0 ){
    digitalWrite(relay, HIGH);
    lcd.setCursor(0,0);
   lcd.print("  MOTOR IS IN   ");
   lcd.setCursor(0,1);
   lcd.print("  ON CONDITION  ");       
  }
   if( sms.indexOf("OFFMOTOR") >= 0 ){
    digitalWrite(  relay, LOW );
    lcd.setCursor(0,0);
   lcd.print("  MOTOR IS IN   ");
   lcd.setCursor(0,1);
   lcd.print(" OFF CONDITION  "); 
   a=1;
   delay(3000);
     }
}
// ###################################################################################################################################
void GprsReadSmsStore( String SmsStorePos ){

 Serial.print( "AT+CMGR=" );
  Serial.println( SmsStorePos );
}
// ###################################################################################################################################
void ProcessGprsMsg() {

  if( msg.indexOf( "Call Ready" ) >= 0 ){
      Serial.println( "AT+CMGF=1" );
  }
 if( msg.indexOf( "+CMTI" ) >= 0 ){
     Serial.println( "*** SMS Received ***" );
      int iPos = msg.indexOf( "," );
     String SmsStorePos = msg.substring( iPos+1 );
     Serial.print( "SMS stored at " );
     Serial.println( SmsStorePos );     
     GprsReadSmsStore( SmsStorePos );
  }
  if( msg.indexOf( "+CMGR:" ) >= 0 ){
      SmsContentFlag = 1;
       msg = "";
    return;
  }
  if( SmsContentFlag == 1 ){
    Serial.println( "*** SMS MESSAGE CONTENT ***" );
    Serial.println( msg );
    Serial.println( "*** END OF SMS MESSAGE ***" );
    ProcessSms( msg );
  }
   msg = "";
  SmsContentFlag = 0;
  // Moisture Sensor Ends
  // Buzzer Begins
   tone(buzzerPin, 50);
  delay(50);
  noTone(buzzerPin);
  delay(100);
  // Buzzer Ends
  //Lcd Starts
    // setting the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 1);
  // print the number of seconds since reset:
  lcd.print(millis() / 1000);
  //GSM Starts

  //GSM Ends
  }
