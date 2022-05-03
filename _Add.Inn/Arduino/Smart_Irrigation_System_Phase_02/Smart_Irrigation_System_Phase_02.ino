// ##################################################################################################################################
#include<DHT.h>
#include <Wire.h>
#include<EEPROM.h>
#include <RTClib.h>
#include <LiquidCrystal.h>
#include <Keypad.h>
#include <SoftwareSerial.h>

#define DHTPIN 10
#define DHTTYPE DHT11

RTC_DS1307 RTC;

DHT dht(DHTPIN, DHTTYPE);

float calibration_value = 21.21;
int ph_analog = A2;
float ph_analog_val;
int led = 13;
unsigned long int avgval;
int buffer_arr[10], temp;

LiquidCrystal lcd(12, 11, 6, 5, 4, 3);// Pins used for RS,E,D4,D5,D6,D7

int moistPin = A0;
float moistValue; 
float limit = 60;

const int buzzerPin = 2;

const byte ROWS = 4; //for four rows
const byte COLS = 3; //for three columns
byte rowPins[ROWS] = {A8, A9, A10, A11}; //connect to the row pinouts of the keypad contains four pins
byte colPins[COLS] = {A12, A13, A14}; //connect to the column pinouts of the keypad contains three pins

int value, pos = 0, newTime, prevTime, a = 0, hoursE = 0, delayTIM;

String msg = String(""); // String buffer for the GPRS shield message
int SmsContentFlag = 0;//Set to 1 when the next GSM/GPRS modem contains a SMS message
int relay = 24; //Pin declare for relay control at pin number 24 of arduino mega
char keys[ROWS][COLS] = {

  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

int lastMinute, i = 0, count = 0, num[10], phno[10], HOUR = 0, MINUT = 0, MONTH = 1, SECOND = 0; //Variable to store phone number

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
// ###########################################################################################################
void setup() {

  Serial1.begin(9600);   // Setting the baud rate of GSM Module
  Serial.begin(9600);    // Setting the baud rate of Serial Monitor (Arduino)
  delay (1000);
  pinMode( relay, OUTPUT );
  digitalWrite( relay, LOW );
  Serial1.println( "AT+CMGF=1" );
  Serial1.println("Preparing to send SMS");
  SendMessage();
  delay(200);
  
  Wire.begin();
  RTC.begin();
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Smart Irrigation");
  lcd.setCursor(0, 1);
  lcd.print("MOISTURE DETECT.");
  delay(1000);
  if (!RTC.isrunning())
  {
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.println("ENTER MOBILE NO.");
  lcd.setCursor(0, 1);
  lcd.println(" YES*       NO# ");

DateTime now = RTC.now();
  prevTime = now.minute();
lcd.print("                ");

  pinMode(ph_analog, INPUT);
  pinMode(13, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
}
// ##################################################################################################################################
void loop() {

  char key = keypad.getKey();
                 while(pos==0){
                  char key = keypad.getKey();
                 if(key=='*' && pos==0){
                  lcd.setCursor(0,0);
                  lcd.println("ENTER MOBILE NO.");
                lcd.setCursor(0,1);
                lcd.println("                ");
                 saveNUM();
               }
               if(key=='#' && pos==0){
                 //sendSMS();
                lcd.clear();
                pos=2;
                break; 
               }
               } 
  // read humidity
  float humi  = dht.readHumidity();
  // read temperature as Celsius
  float tempC = dht.readTemperature();
  // read temperature as Fahrenheit
  float tempF = dht.readTemperature(true);
  // read Ph value of soil
  ph_analog_val = analogRead(ph_analog);
  // read moisture value of soil
  moistValue = analogRead(moistPin);

  // checking if any reads failed
  if (isnan(humi) || isnan(tempC) || isnan(tempF) || isnan(ph_analog_val) || isnan(moistValue)) {
    Serial.println("Failed to read from Sensors");
  } else {
    Serial.print("Humidity: ");
    Serial.print(humi);
    Serial.print("%");

    Serial.print("  |  ");

    Serial.print("Temperature: ");
    Serial.print(tempC);
    Serial.println("C");

    Serial.print("PH Value - ");
    Serial.print(ph_analog_val);

    Serial.print("  |  ");

    Serial.print("Moisture Value - ");
    Serial.println(moistValue);
    Serial.println();

    if (moistValue < limit) {
      a = 0;
      while (1) {
        digitalWrite(13, HIGH);
          lowAlertSMS();
        lcd.setCursor(1, 1);
        lcd.print(" SOIL MOISTURE  ");
        lcd.setCursor(2, 1);
        lcd.print("  LOW DETECTED  ");
        delay(1000);
      }
      while (1) {
        char SerialInByte;
        moistValue = analogRead(A0);
        if (Serial1.available())
        {
          SerialInByte = (unsigned char)Serial1.read();
          delay(5);
          if ( SerialInByte == 13 ) {
                       ProcessGprsMsg();
            Serial.println( "*** SMS Received ***" );
          }
          if ( SerialInByte == 10 ) {
          }
          else {
            msg += String(SerialInByte);
          }
        }
        if (a == 1 || moistValue <= limit) {
          digitalWrite(  relay, LOW );
          lcd.setCursor(0, 0);
          lcd.print("  MOTOR IS IN   ");
          lcd.setCursor(0, 1);
          lcd.print(" OFF CONDITION  ");
          lcd.clear();
          break;
        }
      }
    }
    else {
      digitalWrite(13, LOW);
    }
    delay(1000);
  }
  if (key == '1') {
    moistureDETECT();
  }
  if (key == '2') {
    detectNUM();
  }
  if (key == '0') {
    lcd.setCursor(0, 0);
    lcd.print("ENTER MOBILE NO.");
    lcd.setCursor(0, 1);
    lcd.print("                ");
               saveNUM();
  }
  if (key == '#') {
    lcd.clear();
    timerSET();
  }
  if (key == '3') {
    lcd.clear();
    value = EEPROM.read(11);
    lcd.setCursor(0, 0);
    lcd.print("  TIME SET FOR  ");
    lcd.setCursor(0, 1);
    lcd.print("  EVERY   HOUR   ");
    lcd.setCursor(8, 1);
    lcd.print(value);
    delay(3000);
  }
  if (key == '*') {
    lcd.clear();
    for (int i = 1; i < 41; i++) {
      Serial1.print( "AT+CMGD=" );
      Serial1.println(i);

      lcd.setCursor(0, 0);
      lcd.print(" DELETING SIM   ");
      lcd.setCursor(5, 1);
      lcd.print("MSGS");
      lcd.setCursor(10, 1);
      lcd.print(i);
      delay(500);
    }
  }
  delay(1000);

  lcd.setCursor(8, 0);
  lcd.print(" ");
  lcd.setCursor(10, 1);
  lcd.print(" ");
  dht.read(DHTPIN);
  
  /*DateTime now = RTC.now();
  lcd.setCursor(0, 0);
  printDigits2(HOUR = now.hour());
  lcd.print(":");
  printDigits2(MINUT = now.minute());
  newTime = now.minute();
  lcd.setCursor(5, 0);
  lcd.print("|");
  lcd.setCursor(6, 0);
  lcd.print(now.year(), DEC);
  lcd.print("-");
  lcd.setCursor(11, 0);
  printDigits2(MONTH = now.month());
  lcd.print("-");
  lcd.print(now.day(), DEC); */
  
 /* lcd.setCursor(0, 1);
  lcd.print("Temperature:");  
  lcd.print(analogRead(DHTPIN)); 
  lcd.setCursor(17, 1);
  lcd.print((char)223);
  lcd.print("C"); */
  lcd.setCursor(0, 0);
  moistValue = map(analogRead(moistPin), 550,0,0,100);
  if (analogRead(moistPin)>550 | analogRead(moistPin)<0)
  {
     lcd.print("Moist Val:");
    lcd.print(abs(moistValue)); 
   lcd.println("%");
  }
  else
  {
  lcd.print("Moist Val:");
   lcd.print(moistValue);
   lcd.println("%");
  }

for(int i=0; i<10; i++)
{
  buffer_arr[i]=analogRead(A1);
  delay(30);
}
for(int i=0; i<9; i++)
{
  for(int j=i+1; j<10; j++)
  {
    if(buffer_arr[i]>buffer_arr[j])
    {
      temp=buffer_arr[i];
      buffer_arr[i]=buffer_arr[j];
      buffer_arr[j]=temp;
    }
  }
}
avgval=0;
for(int i=2;i<8;i++)
avgval+=buffer_arr[i];
float volt=(float)avgval*5.0/1024/6;
float ph_analog_val = -5.70 * volt + calibration_value;
  
  lcd.setCursor(0, 1);
   lcd.print("PH Value: ");
   lcd.print(abs(ph_analog_val));
  matchTIM();
}
//#############################################################
void saveNUM() {
  while (1) {
    char key = keypad.getKey();
    if (key) {
      //Serial.println(key);
      phno[i] = key;
      num[i] = phno[i] - 48;
      lcd.setCursor(13, 1);
      lcd.print("    ");
      lcd.setCursor(i + 3, 1);
      lcd.print(num[i]);
      count++;
      i++;
      delay(10);

      if (key == '#' && count == 11) {
        for (int j = 0; j < 11; j++) {
          lcd.setCursor(0, 0);
          lcd.print("***** WAIT *****");
          EEPROM.write(j, num[j]);
          delay(500);
        }
        lcd.setCursor(0, 0);
        lcd.print("  NUMBER SAVED  ");
        lcd.setCursor(0, 1);
        lcd.print("***************");
        lcd.clear();
        pos = 1;
        break;
      }
    }
  }
}
// #################################################################
void printDigits2(int digits)  //this void function is really useful; it adds a "0" to the beginning of the number, so that 5 minutes is displayed as "00:05:00", rather than "00:5 :00"

{
  if (digits < 10)
  {
    lcd.print("0");
    lcd.print(digits);
  }
  else
  {
    lcd.print(digits);
  }
}
// ##########################################################################
void printDigits3(int digits)  //this void function is really useful; it adds a "0" to the beginning of the number, so that 5 minutes is displayed as "00:05:00", rather than "00:5 :00"
{
  if (digits < 1000)
  {
    lcd.print(" ");
    lcd.print(digits);
  }
  else
  {
    lcd.print(digits);
  }
}
// #############################################################################################
void moistureDETECT() {

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Moisture Content");
  while (1) {
    char key = keypad.getKey();
    moistValue = analogRead(A0);
    lcd.setCursor(6, 1);
    printDigits3(moistValue);
    if (key == '*') {
      lcd.clear();
      break;
    }
  }
}
// #############################################################################################################################
void detectNUM() {

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("   OWNER's NO.   ");
  for (int j = 0; j < 11; j++) {
    value = EEPROM.read(j);
    lcd.setCursor(j + 3, 1);
    lcd.print(value);
    lcd.setCursor(13, 1);
    lcd.print("    ");
    delay(300);
  }

  while (1) {

    char key = keypad.getKey();
    if (key == '*') {
      lcd.clear();
      break;
    }
  }
}
// #######################################################################################################
void timerSET() {

  int k = 0;
  lcd.setCursor(0, 0);
  lcd.print("SET TIME FOR: ");

  while (1) {

    char key = keypad.getKey();
    lcd.setCursor(13, 0);
    while (pos == 2) {
      DateTime now = RTC.now();
      char key = keypad.getKey();
      if (key) {
        lcd.print(key);
        delayTIM = key - 48;
        prevTime = now.minute();
        EEPROM.write(12, prevTime);
        delay(500);
        pos = 3;

        break;

      }
    }

    if (pos == 3 && key == '#') {
      EEPROM.write(11, delayTIM);
      lcd.setCursor(0, 0);
      lcd.print("DELAY FOR   HOUR");
      lcd.setCursor(10, 0);
      lcd.print(delayTIM);
      delay(500);
      lcd.clear();
      pos = 2;

      break;

    }
  }
}
// #######################################################################################
void matchTIM() {

  DateTime now = RTC.now();
  hoursE = EEPROM.read(11);
  if (newTime == prevTime + hoursE) {
    value = EEPROM.read(11);
    prevTime = now.minute();
    //while(1){
    //char key = keypad.getKey();
    lcd.setCursor(0, 0);
    lcd.print("  TIME MATCHED  ");
    lcd.setCursor(0, 1);
    lcd.print("  FOR    HOUR    ");
    lcd.setCursor(7, 1);
    lcd.print(value);
       moistureAlertSMS();
    lcd.clear();
    delay(1000);
  }
    }
// ###########################################################################
void lowAlertSMS() {

  delay(100);
  Serial1.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1000 milli seconds or 1 second
  //mySerial.println("AT+CMGS="+255783889987"r"); // Replace x with mobile number
  Serial1.print("AT+CMGS=\"+255745100699\"");
  for (int j = 0; j < 10; j++) {
    value = EEPROM.read(j);
    Serial1.print(value);
    delay(3);
  }
  Serial1.println("""r");
  delay(1000);
  Serial1.println("Alert !!! Low moisture level Detected");// The SMS text you want to send
  delay(100);
  Serial1.println((char)26);// ASCII code of CTRL+Z
  delay(1000);
}

//################################################################################################################################
void moistureAlertSMS() {
  moistValue = analogRead(A0);
  delay(100);
  Serial1.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1000 milli seconds or 1 second
  //mySerial.println("AT+CMGS="+255783889987"r"); // Replace x with mobile number
  Serial1.println("AT+CMGS=\"+255745100699\"");
  for (int j = 0; j < 10; j++) {
    value = EEPROM.read(j);
    Serial1.print(value);
    delay(3);
  }
  Serial1.println("""r");
  delay(1000);
  Serial1.print("Soil Moisture is:");// The SMS text you want to send
  delay(100);
  Serial1.println(moistValue);// The SMS text you want to send
  delay(100);
  Serial1.println((char)26);// ASCII code of CTRL+Z
  delay(1000);
}
// ##################################################################################################################
void ProcessSms( String sms ) {

  if ( sms.indexOf("ONMOTOR") >= 0 ) {
    digitalWrite( relay, HIGH );
    lcd.setCursor(0, 0);
    lcd.print("  MOTOR IS IN   ");
    lcd.setCursor(0, 1);
    lcd.print("  ON CONDITION  ");
  }
  if ( sms.indexOf("OFFMOTOR") >= 0 ) {
    digitalWrite(  relay, LOW );
    lcd.setCursor(0, 0);
    lcd.print("  MOTOR IS IN   ");
    lcd.setCursor(0, 1);
    lcd.print(" OFF CONDITION  ");
    a = 1;
    delay(3000);
  }
}
// ###################################################################################################################
void GprsReadSmsStore( String SmsStorePos ) {

  Serial1.print( "AT+CMGR=" );
  Serial1.println( SmsStorePos );
}
// ################################################################################################################################
void ProcessGprsMsg() {

  if ( msg.indexOf( "Call Ready" ) >= 0 ) {
    Serial1.println( "AT+CMGF=1" );
  }
  if ( msg.indexOf( "+CMTI" ) >= 0 ) {
    Serial.println( "*** SMS Received ***" );
    int iPos = msg.indexOf( "," );
    String SmsStorePos = msg.substring( iPos + 1 );
    Serial.print( "SMS stored at " );
    Serial.println( SmsStorePos );
    GprsReadSmsStore( SmsStorePos );
  }
  if ( msg.indexOf( "+CMGR:" ) >= 0 ) {
    SmsContentFlag = 1;
    msg = "";
    return;
  }
  if ( SmsContentFlag == 1 ) {
    Serial.println( "*** SMS MESSAGE CONTENT ***" );
    Serial.println( msg );
    Serial.println( "*** END OF SMS MESSAGE ***" );
    ProcessSms( msg );
  }
  msg = "";
  SmsContentFlag = 0;
}

void SendMessage()
{
   Serial1.println("Setting the GSM in text mode");
   Serial1.println("AT+CMGF=1\r");
   delay(2000);
   Serial1.println("Sending SMS to the desired phone number!");
   Serial1.println("AT+CMGS=\"+xxxxxxxxxxx\"\r");
   // Replace x with mobile number
   delay(2000);

   Serial1.println("Hello from SIM900");    // SMS Text
   delay(200);
   Serial1.println((char)26);               // ASCII code of CTRL+Z
   delay(2000);
}
// ###################################################################################################################################
