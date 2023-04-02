#include <Arduino.h>
#include <U8g2lib.h>
#include "global.h"
U8G2_ST7920_128X64_F_HW_SPI lcd (U8G2_R0, 5, 22);




void setup() {
  lcd.setBusClock(500000); 
  lcd.begin();
  lcd.enableUTF8Print();
}

void loop() {
  lcd.firstPage();

  do {
    mainPage();
  }while ( lcd.nextPage() );
}


void mainPage(void){

}