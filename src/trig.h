#ifndef _TRIG_H_
#define _TRIG_H_
#include <Arduino.h>

class Trig{
    public:
    explicit Trig(int pin, const unsigned long timeout, unsigned long millis): pin(pin), timeout(timeout){
        preMillis = millis;
        out = false;
    }
    void run(unsigned long millis){
       if(millis - preMillis >= timeout){
            preMillis = millis;
            out = false;
            int readPin = digitalRead(pin);
            if(preState != readPin){
                preState = readPin;
                if(!readPin){
                    out = true;
                }
            }
       } 
    }
    bool get(void){
        return out;
    }
    private:
        bool                out;
        bool                preState;
        int                 pin;
        const unsigned long timeout;
        unsigned long       preMillis;
};
#endif // _TRIG_H_