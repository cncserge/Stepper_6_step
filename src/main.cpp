#include <Arduino.h>
#include <U8g2lib.h>
#include <Keypad.h>
/* lcd connect
RS  -> 10
RW  -> 11
EN  -> 13
RST -> 12
BLA -> 3v3
PCB -> GND
*/

void oneStepPage(void);
void allStepPage(void);
void navigationPrc(void);
enum{page_all, page_step};
enum{mode_run, mode_stop, mode_edit};


//U8G2_ST7920_128X64_F_HW_SPI lcd (U8G2_R0, 10, 12);
U8G2_ST7920_128X64_1_HW_SPI lcd (U8G2_R0, 10, 12);
bool blink;
int  currentEditStep = 0;
int pageCurrent;
int mode;
double stepsValue[6];
char key;
char hexaKeys[4][4] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[4] = {6, 7, 8, 9}; //connect to the row pinouts of the keypad
byte colPins[4] = {2, 3, 4, 5}; //connect to the column pinouts of the keypad
Keypad keyPad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, 4, 4); 


void setup() {
  Serial.begin(9600);
  //lcd.setBusClock(500000); 
  lcd.begin();
  lcd.enableUTF8Print();
  pageCurrent = page_step;
  mode = mode_edit;
  for(int i = 0; i < 6; i ++)
    stepsValue[i] = i + 0.1;
}

void loop() {
  lcd.firstPage();

  do {
    if(pageCurrent == page_all)  allStepPage();
    if(pageCurrent == page_step) oneStepPage();
  }while ( lcd.nextPage() );



  {
    static unsigned long t = millis();

    if(millis() - t >= 400UL){
      t = millis();
      if(blink)
        blink = false;
      else
        blink = true;
    }
  }

  key = keyPad.getKey();
  
  navigationPrc();


}

void navigationPrc(void){
  static int firstEdit = 0;
  static String tempStr;

  if(firstEdit == 0) tempStr = "";

  if(key == 'A' && currentEditStep > 0){
    currentEditStep --;
    firstEdit = 0;
  }
  if(key == 'B' && currentEditStep < 5){
    currentEditStep ++;
    firstEdit = 0;
  }

  if(mode == mode_edit){
    if(tempStr.toDouble() < 1000.0){
      if(key >= '0' && key <= '9'){
        Serial.println(tempStr.indexOf('.'));
        firstEdit++;
        tempStr += key;
      }
      if(key == '*'){
        if(tempStr.indexOf('.') == -1){
          firstEdit++;
          tempStr += '.';
        }
      }
    }

    if(key == 'C'){
      tempStr = "";
      stepsValue[currentEditStep] = 0;
    }  

    if(key != NO_KEY){
      if(firstEdit > 0){
        stepsValue[currentEditStep] = tempStr.toDouble();
      }   
    }
  }










}

void oneStepPage(void){
  lcd.setFont(u8g2_font_6x13B_t_cyrillic);
  lcd.drawUTF8(40, 10, String("Крок - " + String(currentEditStep + 1)).c_str());
  lcd.setFont(u8g2_font_inb24_mn);
  lcd.setCursor(0, 42);
  lcd.print(stepsValue[currentEditStep], 1);
}

void allStepPage(void){
  lcd.setFont(u8g2_font_5x8_t_cyrillic);
  for(int i = 0; i < 6; i++){
    if((currentEditStep == i)){
      lcd.drawUTF8(0, (i * 10) + 10, String(" -> Крок " + String(i + 1) + " ---  " + String(stepsValue[i], 1)).c_str());
    }
    else{
      lcd.drawUTF8(0, (i * 10) + 10, String("    Крок " + String(i + 1) + " ---  " + String(stepsValue[i], 1)).c_str());
    }
  }

}