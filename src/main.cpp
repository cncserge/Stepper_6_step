#include <Arduino.h>
#include <U8g2lib.h>
#include <Keypad.h>
#include "global.h"
/* lcd connect
RS  -> 10
RW  -> 11
EN  -> 13
RST -> 12
BLA -> 3v3
PCB -> GND
*/

U8G2_ST7920_128X64_F_HW_SPI lcd (U8G2_R0, 10, 12);

char hexaKeys[4][4] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[4] = {3, 2, 1, 0}; //connect to the row pinouts of the keypad
byte colPins[4] = {7, 6, 5, 4}; //connect to the column pinouts of the keypad
Keypad keyPad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, 4, 4); 


void setup() {
  lcd.setBusClock(500000); 
  lcd.begin();
  lcd.enableUTF8Print();
  //lcd.drawCircle(64, 64, 10);
}

void loop() {
  lcd.firstPage();

  do {
    mainPage();
  }while ( lcd.nextPage() );
}


void mainPage(void){
   lcd.drawCircle(64, 32, 10);
}