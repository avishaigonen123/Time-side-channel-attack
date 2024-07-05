#include <Arduino.h>

namespace Touch{

    #define RESET_BUTTON 0x001
    #define ENTER_BUTTON 0x100
    #define INITIALIZE_VALUE 0x909
    #define TOUCH_DELAY 5

    uint16_t state = 0;
    const uint8_t real_to_fake_keypad[10] = {4, 3, 7, 11, 2, 6, 10, 1, 5 ,9};

    void relaese(){
        state = 0;
        delay(TOUCH_DELAY);
    }

    void touch(uint8_t num){

        state = 1 << real_to_fake_keypad[num];
        delay(TOUCH_DELAY);
    }

    void reset(){
        
        state = RESET_BUTTON;
        delay(TOUCH_DELAY);
    }

    void enter(){
        
        state = ENTER_BUTTON;
    }

    volatile uint8_t data[4];

}