#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "TimeAttack.h"
#include "I2cTask.h"
#include "TFTTask.h"

void setup() {
    Serial.begin(115200);

	xTaskCreatePinnedToCore(
        I2CTask::i2cTask, 
        "i2cTask", 
        10000, 
        NULL, 
        1, 
        NULL,
        0
    );

    xTaskCreatePinnedToCore(
		TFT::TFTTask,
        "TFT",
        5000,
        NULL,
        20,
        &(TSCA::tftTaskHandle),
		1
	);

	xTaskCreatePinnedToCore(
		TSCA::TSCALoop,
        "TSCA",
        5000,
        NULL,
        100,
        NULL,
		1
	);

    vTaskDelete(NULL);
}


void loop(){}
