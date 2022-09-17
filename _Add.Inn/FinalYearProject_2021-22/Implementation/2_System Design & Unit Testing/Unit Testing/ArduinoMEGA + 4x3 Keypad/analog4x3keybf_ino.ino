/* program Analog4x3Keybf
 *  test for 4x3 keys keyboard with 3 analog outs
 *  G. Carrera - 19/11/2018
 */
// limits of keyboard output values:
const int NADCm100[4] = {923,669,412,156};
const int NADCp100[4] = {1123,869,612,356};
const char key[13] = {'1','4','7','C','2','5','8','0','3','6','9','E'};
int keyval[3];
int i,colp,val;


void setup(){
 Serial.begin(9600); // used with serial monitor
}

void loop() {
  char k;
  k = KeyPad();// read keypad
  if (k != 'N'){ // a key was pressed
          Serial.print("key = ");
          Serial.println(k);
          delay(500);
  }
}

/******** Functions used by program ********/
char KeyPad(){
// read keypad and return the char key
// key = 'N' for none
  KeyScan();// read analog keyboard
  if (keyval[0]+keyval[1]+keyval[2] < 40) {
    return 'N';
  }
  else { // a key was pressed
    delay(10);// antibounce
    KeyScan();// reread analog keyboard
    for (i=0; i < 3; i++){//identify which column it belongs to
      if (keyval[i] > 40){
        colp= i;
        val= keyval[i];// this is the corresponding value
        for (int j=0; j < 4; j++){// identify which key was pressed on the column
          if (val >= NADCm100[j] && keyval <= NADCp100[j]){ 
            return key[colp*4+j];
            break;
          }
        }
      }  
    }
  }
}


void KeyScan(){// read analog keyboard
  keyval[0]= analogRead(A0);
  delay(1);
  keyval[1]= analogRead(A1);
  delay(1);
  keyval[2]= analogRead(A2);
  delay(1);  
}
