#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "TimeAttack.h"
#include "esp_event.h"
#include <TFT_eSPI.h>

using namespace TSCA;

esp_event_loop_handle_t eventLoopHandle;

void TFTTask(void* arg){
    TFT_eSPI tft = TFT_eSPI();
    tft.begin();
    tft.setRotation(6);
    tft.fillScreen(TFT_BLACK);

    tft.drawCircle(128/2, 128/2, 50, TFT_GREENYELLOW);
    byte color = 0;
    while (true){
        for (size_t i = 0; i <= 360; i+=1){
            tft.drawArc(128/2, 128/2, 50, 45, 0, i, TFT_GREENYELLOW + 0x1111 * color, TFT_BLACK);
            ulTaskNotifyTake(pdTRUE, 1 / portTICK_PERIOD_MS);
            if(TSCA::eventState == FOUND_I2C)
                goto exit;
        }
        color++;
    }
    exit:
    tft.drawArc(128/2, 128/2, 50, 45, 0, 360, TFT_GREENYELLOW, TFT_BLACK);
    char buf[PASS_SIZE + 1]; // Create a buffer with an extra space for the null-terminator
    static const uint16_t textColor = TFT_WHITE;
    static const uint16_t backgroundColor = TFT_BLACK;
    static const uint16_t finishTextColor = TFT_BLUE;
    static const uint16_t finishBackgroundColor = TFT_GOLD;

    const int textX = 128 / 2;
    const int textY = (128 / 2) + 5;
    const int textWidth = 60;  // Adjust width according to your text size
    const int textHeight = 20; // Adjust height according to your text size

    tft.setTextSize(2);  // Set text size
    tft.setTextColor(textColor, backgroundColor); // Set text color and background color
    tft.drawCentreString("XXXX", textX, textY - textHeight / 2, 1);
    uint8_t index = 1;
    //vTaskDelete(NULL);
     for (;;) {
        // Wait for notification from TSCALoop
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        //Serial.println("hi");
        switch (TSCA::eventState)
        {
        case FINISHED_BATCH:
            for (size_t i = 0; i < index; i++) {
                buf[i] = '0' + pass[i];
            }
            for (size_t i = index; i < PASS_SIZE; i++) {
                buf[i] = 'X';
            }
            buf[PASS_SIZE] = '\0'; // Null-terminate the string

            // Draw the string on the TFT screen
            tft.drawCentreString(buf, textX, textY - textHeight / 2, 1);

            index++;
            break;
        case SUCCESS:
            for (size_t i = 0; i < PASS_SIZE; i++) {
                buf[i] = '0' + pass[i];
            }
            buf[PASS_SIZE] = '\0'; // Null-terminate the string

            // Draw a rectangle with the finish background color
            tft.setTextColor(finishTextColor, finishBackgroundColor);
            tft.fillRect(textX - textWidth / 2, textY - textHeight / 2 - 3, textWidth, textHeight, finishBackgroundColor);

            // Draw the final string on the TFT screen
            tft.drawCentreString(buf, textX, textY - textHeight / 2, 1);
            tft.drawArc(128/2, 128/2, 50, 45, 0, 360, TFT_BLUE, TFT_BLACK);
            break;
        case TSCA::FAIL:
            index = 1;
            tft.drawArc(128/2, 128/2, 50, 45, 0, 360, TFT_GREENYELLOW, TFT_BLACK);
            tft.drawCentreString("XXXX", textX, textY - textHeight / 2, 1);
            break;
        case INC:
            tft.drawArc(128/2, 128/2, 50, 45, 0, (uint16_t)(progress * 3.6), TFT_BLUE, TFT_BLACK);
            break;
        
        default:
            break;
        }
        eventState = NOTHING;
    }

    vTaskDelete(NULL);
}

void setup() {
    Serial.begin(115200);

    esp_event_loop_args_t eventLoopArgs = {
        .queue_size = 5,
        .task_name = "eventLoop",
        .task_priority = 1,
        .task_stack_size = 1000,
        .task_core_id = 0
    };
    
    esp_event_loop_create(&eventLoopArgs, &eventLoopHandle);
    
    
    xTaskCreatePinnedToCore(
		TFTTask,
        "TFT",
        5000,
        NULL,
        20,
        &(TSCA::tftTaskHandle),
		1
	);

	xTaskCreatePinnedToCore(
		TSCALoop,
        "TSCA",
        5000,
        NULL,
        100,
        &(TSCA::TSCATaskHandle),
		1
	);


	xTaskCreatePinnedToCore(
        i2cTask, 
        "i2cTask", 
        10000, 
        NULL, 
        1, 
        NULL,
        0
    );
    
    vTaskDelete(NULL);
}


void loop(){}