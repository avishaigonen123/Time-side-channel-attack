#pragma once
#include <Arduino.h>
#include <Wire.h>
#include "touchHandle.h"
#include "TimeAttack.h"
#include "StopWatch.h"

namespace I2CTask {
    volatile uint8_t rBuffer[4];

    void onRec_Callback(int numOfBytes){
        for(uint8_t i = 0 ; Wire.available() ; i++){
            rBuffer[i] = Wire.read();
        }
    }

    void onReq_Callback(){
        if(rBuffer[0] == 0x5E){
            Wire.write(0x24);
        }
        else if(rBuffer[0] == 0x5D){
            Wire.write(0x0);
        }
        else if(rBuffer[0] == 0x00){
            Wire.write((uint8_t)Touch::state);
            Wire.write(Touch::state >> 8);
            if(Touch::state == ENTER_BUTTON){
                StopWatch::startTimer();
            }
        }
        rBuffer[0] = 0xFF;
    }

    void i2cTask(void *parameter) {
        memset((void*)rBuffer, 0xFF, sizeof(rBuffer));
        
        while(!Wire.begin(0x5A, SDA, SCL, 400000)){delay(10);}
        Wire.onReceive(onRec_Callback);
        Wire.onRequest(onReq_Callback);

        TSCA::eventState = TSCA::FOUND_I2C;
        
        xTaskNotifyGive(xTaskGetHandle("TSCA"));
        log_d("notified TSCA");

        vTaskDelete(NULL);
    }

}