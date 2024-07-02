#include <Arduino.h>
#include <Wire.h>
#include "touchHandle.h"

namespace I2CTask{
    typedef struct I2CTaskArgs{
        uint8_t sup;
    } I2CTaskArgs;

    void onRec_Callback(int numOfBytes){
        for(uint8_t i = 0 ; Wire.available() ; i++){
            Touch::data[i] = Wire.read();
        }
    }

    void onReq_Callback(){
        if(Touch::data[0] == 0x5E){
            Wire.write(0x24);
        }
        else if(Touch::data[0] == 0x5D){
            Wire.write(0x0);
        }
        else if(Touch::data[0] == 0x00){
            Wire.write((uint8_t)Touch::state);
            Wire.write(Touch::state >> 8);
            if(Touch::state == ENTER_BUTTON){
                startTimer();
            }
        }
        Touch::data[0] = 0xFF;
    }

    void i2cTask(void* arg) {

        I2CTaskArgs* i2cArgs = (I2CTaskArgs*)arg;

        while(!Wire.begin(0x5A, SDA, SCL, 400000)){delay(10);}

        for (uint8_t i = 0; i < 4; i++){
            Touch::data[i] = 0xFF;
        }

        Wire.onReceive(onRec_Callback);
        Wire.onRequest(onReq_Callback);

        TSCA::eventState = FOUND_I2C;
        xTaskNotifyGive(tftTaskHandle);
        xTaskNotifyGive(TSCATaskHandle);

        vTaskDelete(NULL);
    }
}