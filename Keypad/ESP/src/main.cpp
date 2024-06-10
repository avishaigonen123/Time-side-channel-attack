#include <Arduino.h>
#include <pins_arduino.h>
#include <Wire.h>

#define FAIL_PIN 27
#define SUCCESS_PIN 14
#define PASS_SIZE 4

#define ROW_0 GPIO_NUM_19
#define ROW_1 GPIO_NUM_18
#define ROW_2 GPIO_NUM_5
#define ROW_3 GPIO_NUM_17
#define COL_0 GPIO_NUM_16
#define COL_1 GPIO_NUM_4
#define COL_2 GPIO_NUM_15

#define RESET_BUTTON 10
#define ENTER_BUTTON 11
#define TOUCH_DELAY 5

uint8_t state = 0;

void printCol(){
    Serial.printf("%d %d %d\n",
        digitalRead(COL_0),
        digitalRead(COL_1),
        digitalRead(COL_2));
}

uint8_t rowMap(uint8_t num){
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
        return -1;
    }
}

uint8_t colMap(uint8_t num){
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
        return -1;
    }
}

bool readyToSend(uint8_t col)
{
    switch (col)
    {
        case COL_0:
            return !digitalRead(COL_0) && digitalRead(COL_1) && digitalRead(COL_2);
        case COL_1:
            return digitalRead(COL_0) && !digitalRead(COL_1) && digitalRead(COL_2);
        case COL_2:
            return digitalRead(COL_0) && digitalRead(COL_1) && !digitalRead(COL_2);
    }
    return false;               
}

void touch(uint8_t num){
    while(!readyToSend(colMap(num)));
    digitalWrite(rowMap(num), LOW);
}

void reset(){
	touch(RESET_BUTTON);
}

void enter(){
	touch(ENTER_BUTTON);
}

void release(){
    digitalWrite(ROW_0, HIGH);
    digitalWrite(ROW_1, HIGH);
    digitalWrite(ROW_2, HIGH);
    digitalWrite(ROW_3, HIGH);
}

void setup() {
    Serial.begin(115200);
    pinMode(FAIL_PIN, INPUT);
    pinMode(SUCCESS_PIN, INPUT);

    pinMode(ROW_0, OUTPUT);
    pinMode(ROW_1, OUTPUT);
    pinMode(ROW_2, OUTPUT);
    pinMode(ROW_3, OUTPUT);
    pinMode(COL_0, INPUT);
    pinMode(COL_1, INPUT);
    pinMode(COL_2, INPUT);
    
    release();

}

void loop(){
    for (size_t i = 0; i < 10; i++)
    {
        touch(5);
        delay(1);
        release();
        delay(10);
        //Serial.println(i);
    }
    
    //delay(10);
    //relaese();
    //printCol();
    
    //delay(0);
}
