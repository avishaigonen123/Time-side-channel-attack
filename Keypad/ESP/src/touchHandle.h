#include <Arduino.h>

namespace keypad{
    #define ROW_0 GPIO_NUM_19
    #define ROW_1 GPIO_NUM_18
    #define ROW_2 GPIO_NUM_5
    #define ROW_3 GPIO_NUM_17
    #define COL_0 GPIO_NUM_16
    #define COL_1 GPIO_NUM_4
    #define COL_2 GPIO_NUM_15
    #define NO_KEY GPIO_NUM_2 

    #define START_PIN GPIO_NUM_26

    #define RESET_BUTTON 10
    #define ENTER_BUTTON 11
    #define TOUCH_DELAY 5

    byte col = COL_0;
    byte row = ROW_2;

    void printCol(){
        Serial.printf("%d %d %d\n",
            digitalRead(COL_0),
            digitalRead(COL_1),
            digitalRead(COL_2));
    }

    byte rowMap(byte num){
        switch (num)
        {
        case 1:
        case 2:
        case 3:
            return ROW_0;
        case 4:
        case 5:
        case 6:
            return ROW_1;
        case 7:
        case 8:
        case 9:
            return ROW_2;
        case RESET_BUTTON:
        case 0:
        case ENTER_BUTTON:
            return ROW_3;
        default:
            return NO_KEY;
        }
    }

    byte colMap(byte num){
        switch (num)
        {
        case 1:
        case 4:
        case 7:
        case RESET_BUTTON:
            return COL_0;
        case 2:
        case 5:
        case 8:
        case 0:
            return COL_1;
        case 3:
        case 6:
        case 9:
        case ENTER_BUTTON:
            return COL_2;
        default:
            return NO_KEY;
        }
    }

    void inline waitForChange(){
        while(!digitalRead(COL_0) | !digitalRead(COL_1) | !digitalRead(COL_2)){
            yield();
        }
    }

    void touch(byte num){
        waitForChange();
        row = rowMap(num);
        col = colMap(num);
    }


    void inline release(){
        waitForChange();
        row = NO_KEY;
        col = NO_KEY;
        digitalWrite(ROW_0, HIGH);
        digitalWrite(ROW_1, HIGH);
        digitalWrite(ROW_2, HIGH);
        digitalWrite(ROW_3, HIGH);
    }

    void press(byte num){
        touch(num);
        delay(5);
        release();
        delay(5);
    }

    void inline reset(){
        press(RESET_BUTTON);
    }

    void inline enter(){
        waitForChange();
        //startTimer();
        row = rowMap(ENTER_BUTTON);
        col = colMap(ENTER_BUTTON);
        delay(5);
        release();
        delay(5);
    }
}