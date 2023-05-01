// encoding UTF_8
#include <Arduino.h>
#include <U8g2lib.h>
#include <Keypad.h>
#include <AccelStepper.h>
#include "trig.h"
/* lcd connect
RS  -> 17
RW  -> 19
EN  -> 18
RST -> 20 - 
BLA -> 3v3
PCB -> GND
*/
/*
LCD_RS = Pin(17) # RS / CS -> green
LCD_E = Pin(18) #E / SCK -> orange
LCD_RW = Pin(19) # RW / MOSI -> yellow
*/
const int pinStep = 14;
const int pinDir  = 15;
const int pinEndStop = 12;

const int pinNextStep = 10;
const int pinZero     = 11;
const int pinToHome   = 13;


Trig trigNext(200, millis());
Trig trigZero(200, millis());
Trig trigHome(200, millis());


float speedHome = 5000;
float speedRun  = 12000;
AccelStepper motor(AccelStepper::DRIVER, pinStep, pinDir);
void runPage(void);
void allStepPage(void);
void navigationPrc(void);
enum{page_all, page_run};
enum{mode_run, mode_edit};
bool debug = true;

U8G2_ST7920_128X64_F_HW_SPI lcd (U8G2_R0, 17, 20);
//U8G2_ST7920_128X64_1_HW_SPI lcd (U8G2_R0, 17, 12);
bool blink;
int  currentEditStep = 0;
int  currentRunStep = 0;
int pageCurrent;
int mode;
double stepsValue[6];
const long pulseStepMm = 640;// задаем количество импульсов на милиметр движения
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
  pinMode(pinNextStep , INPUT_PULLUP);
  pinMode(pinZero     , INPUT_PULLUP);
  pinMode(pinToHome   , INPUT_PULLUP);
  pinMode(pinEndStop, INPUT_PULLUP);
  motor.setMaxSpeed(15000);
  motor.setAcceleration(10000);
  Serial.begin(9600);
  //lcd.setBusClock(500000); 
  lcd.begin();
  lcd.enableUTF8Print();
  pageCurrent = page_all;
  mode = mode_edit;
  if(debug){
    for(int i = 0; i < 6; i ++)
      stepsValue[i] = 0;
  }
  
}

void loop() {
  trigNext.run(millis(), digitalRead(pinNextStep));
  trigHome.run(millis(), digitalRead(pinToHome));
  trigZero.run(millis(), digitalRead(pinZero));

  lcd.firstPage();

  do {
    if(pageCurrent == page_all)  allStepPage();
    if(pageCurrent == page_run) runPage();
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
  if(debug){
    if(key != NO_KEY){
      Serial.println(key);
    }
  }



  if(mode == mode_edit){
    pageCurrent = page_all;
  }
  else{
    pageCurrent = page_run;
  }


  
  navigationPrc();


}

void navigationPrc(void){
  {
    if(mode == mode_run){
      { // add moving
        int move = 0;
        if(key == '1'){ // + 10
          move = pulseStepMm * 10;
        }
        if(key == '2'){ // + 1
          move = pulseStepMm * 1;
        }
        if(key == '3'){ // + 0.1
          move = pulseStepMm * 0.1;
        }
        if(key == '4'){ // - 10
          move = -(pulseStepMm * 10);
        }
        if(key == '5'){ // - 1
          move = -(pulseStepMm * 1);
        }
        if(key == '6'){ // - 0.1
          move = -(pulseStepMm * 0.1);
        }
        if(move != 0){
          currentRunStep = 0;
          motor.setCurrentPosition(0);
          motor.moveTo(move);
          while(1){
            motor.run();
            if(!motor.isRunning()){
              motor.setCurrentPosition(0);
              break;
            }
          }
        }
      }
      {
        if(key == 'B'){
          currentRunStep = 0;
          motor.moveTo(stepsValue[currentRunStep] * pulseStepMm);  
          while(1){
            motor.run();
            if(!motor.isRunning()){
              currentRunStep++;
              break;
            }
          }
        }
      }
      {
        if(key == 'D' || trigHome.get()){
          motor.setSpeed(speedHome);
          motor.setCurrentPosition(0);
          motor.moveTo(-(pulseStepMm * 10));
          while(1){
            motor.run();
            if(digitalRead(pinEndStop) == LOW){
              motor.stop();
              motor.setCurrentPosition(0);
              break;
            }
            if(!motor.isRunning()){
              break; // not signal end stop
            }
          }
        }
      }
      {
        if(key == 'C' || trigZero.get()){
          currentRunStep = 0;
          motor.setCurrentPosition(0);
        }
      }
      {
        static bool moveToHome = false;
        if(key == 'A' || trigNext.get()){
          motor.setSpeed(speedRun);
          motor.moveTo(stepsValue[currentRunStep] * pulseStepMm);

          if(abs(stepsValue[currentRunStep]) < 0.1){
            moveToHome = true;
          }
          if(currentRunStep == 6){
            moveToHome = true;
          }
          if(moveToHome){
            currentRunStep = 0;
            motor.moveTo(stepsValue[currentRunStep] * pulseStepMm);  
          }
          while(1){
            motor.run();
            if(!motor.isRunning()){
              if(!moveToHome)
                currentRunStep++;
              if(moveToHome){
                currentRunStep++;
              }
              moveToHome = false;
              break;
            }
          }
        }
      }
    }
  }

  {
    if(key == '#'){
      if(mode == mode_edit){
        mode = mode_run;
      }
      else if(mode == mode_run){
        mode = mode_edit;
      }
    }
  }
  {
    static int firstEdit = 0;
    static String tempStr = "";

    if(mode == mode_edit){

      if(firstEdit == 0) tempStr = "";

      if(key == 'A' && currentEditStep > 0){
        currentEditStep --;
        firstEdit = 0;
      }
      if(key == 'B' && currentEditStep < 5){
        currentEditStep ++;
        firstEdit = 0;
      }


      if(tempStr.toDouble() < 999.0){
        if(key >= '0' && key <= '9'){
          Serial.println(tempStr.indexOf('.'));
          firstEdit++;
          tempStr += key;
        }
      }
      if(key == '*'){
        if(tempStr.indexOf('.') == -1){
          firstEdit++;
          tempStr += '.';
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
    else{
      firstEdit = 0;
      tempStr = "";
    }
  }
}


void runPage(void){
  lcd.setFont(u8g2_font_6x13B_t_cyrillic);
  // if(currentRunStep < 6)
  //   lcd.drawUTF8(40, 10, String("Крок - " + String(currentRunStep + 1)).c_str());
  // else
  //   lcd.drawUTF8(40, 10, "Крок - ZERO");
  lcd.setFont(u8g2_font_inb24_mn);
  lcd.setCursor(0, 42);
  lcd.print(motor.currentPosition() / (double)pulseStepMm, 1);
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