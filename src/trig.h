// encoding UTF_8
#ifndef _TRIG_H_
#define _TRIG_H_
#include <Arduino.h>

class Trig{
    public:
    explicit Trig(const unsigned long timeout, unsigned long millis):  timeout(timeout){
        preMillis = millis;
        out = false;
        preState = true;
    }
    void run(unsigned long millis, int state){
        out = false;
        if(millis - preMillis >= timeout){
                preMillis = millis;
                if(preState != state){
                    preState = state;
                    if(state){
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
        const unsigned long timeout;
        unsigned long       preMillis;
};
#endif // _TRIG_H_