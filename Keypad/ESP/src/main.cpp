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
#define NO_KEY GPIO_NUM_2  // Unused

#define RESET_BUTTON 10
#define ENTER_BUTTON 11
#define TOUCH_DELAY 5

byte Row = 0xff;
byte Col = 0xff;

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

inline byte getTouchMap(){
    return digitalRead(COL_0) | digitalRead(COL_1) << 1 | digitalRead(COL_2) << 2;
}

void release(){
    Row = NO_KEY;
    digitalWrite(ROW_0, HIGH);
    digitalWrite(ROW_1, HIGH);
    digitalWrite(ROW_2, HIGH);
    digitalWrite(ROW_3, HIGH);
}

void touch(byte num){
    Row = rowMap(num);
    Col = colMap(num);
    Serial.print("num: ");
    Serial.println(num);
    digitalWrite(ROW_0, HIGH);
    digitalWrite(ROW_1, HIGH);
    digitalWrite(ROW_2, HIGH);
    digitalWrite(ROW_3, HIGH);
}

void reset(){
    touch(RESET_BUTTON);
}

void enter(){
    touch(ENTER_BUTTON);
}

void Main(void*){
    for(;;){
        if (Row != NO_KEY && Col != NO_KEY) {
            digitalWrite(Row, digitalRead(Col));
        }
        vTaskDelay(1);  // Add a small delay to yield to other tasks
    }
}

void toucher(void*){
    for(;;){
        for (size_t i = 0; i < 10; i++){
            touch(i);
            vTaskDelay(100);
            release();
            vTaskDelay(100);
        }
    }
}

void monit(void*){
    for(;;){
        Serial.print(Row);
        Serial.print(" ");
        if (Row != NO_KEY) {
            Serial.print(digitalRead(Row));
        }
        Serial.print(" ");
        Serial.println(getTouchMap());
        vTaskDelay(1);  // Add a small delay to yield to other tasks
    }
}

void setup() {
    Serial.begin(115200);
    
    pinMode(ROW_0, OUTPUT);
    pinMode(ROW_1, OUTPUT);
    pinMode(ROW_2, OUTPUT);
    pinMode(ROW_3, OUTPUT);
    pinMode(COL_0, INPUT);
    pinMode(COL_1, INPUT);
    pinMode(COL_2, INPUT);

    xTaskCreatePinnedToCore(
        Main,
        "main",
        3000,  // Increase stack size
        NULL,
        21,
        NULL,
        1
    );

    xTaskCreatePinnedToCore(
        monit,
        "monit",
        2000,  // Increase stack size
        NULL,
        20,
        NULL,
        0
    );

    xTaskCreatePinnedToCore(
        toucher,
        "toucher",
        2000,  // Increase stack size
        NULL,
        20,
        NULL,
        0
    );
    release();
    vTaskDelete(NULL);
}

void loop(){}
