#pragma once
#include <Arduino.h>
#include "TimeAttack.h"
#include <TFT_eSPI.h>

#define textColor TFT_WHITE
#define backgroundColor TFT_BLACK
#define finishTextColor TFT_BLUE
#define finishBackgroundColor TFT_GOLD
#define textColor TFT_WHITE

#define textX 128 / 2
#define textY (128 / 2) + 5
#define textWidth 60
#define textHeight 20
#define TextSize 2


namespace TFT{
    
    void drawProggresBar(TFT_eSPI& tft, bool finalPass, uint16_t greenAngle){
        log_d("%d", greenAngle);
        if(!finalPass)
            if(greenAngle<=180){
                tft.drawArc(128/2, 128/2, 50, 45, 180, 180 + greenAngle, TFT_GREENYELLOW, TFT_BLACK);
                tft.drawArc(128/2, 128/2, 50, 45, 180 + greenAngle, 360, TFT_RED, TFT_BLACK);
                tft.drawArc(128/2, 128/2, 50, 45, 0, 180, TFT_RED, TFT_BLACK);
            }
            else{
                tft.drawArc(128/2, 128/2, 50, 45, 180, 0, TFT_GREENYELLOW, TFT_BLACK);
                tft.drawArc(128/2, 128/2, 50, 45, 0, greenAngle - 180, TFT_GREENYELLOW, TFT_BLACK);
                tft.drawArc(128/2, 128/2, 50, 45, greenAngle - 180 , 180, TFT_RED, TFT_BLACK);
            }
        else{
            tft.drawArc(128/2, 128/2, 50, 45, 0, 360, TFT_BLUE, TFT_BLACK);
        }
    }

    void drawProggresBar(TFT_eSPI& tft, bool finalPass){
        drawProggresBar(tft, finalPass, 360);
    }

    void drawPassDigits(TFT_eSPI& tft, char* passDigits, uint8_t passSize, bool finalPass){

        if(finalPass){
            tft.setTextColor(finishTextColor, finishBackgroundColor);
            tft.fillRect(textX - textWidth / 2, textY - textHeight / 2 - 3, textWidth, textHeight, finishBackgroundColor);
        }
        tft.drawCentreString(passDigits, textX, textY - textHeight / 2, 1);

    }
    
    void sleepScrean(TFT_eSPI& tft){
        byte color = 0;
        for(;;){
            for (size_t i = 0; i <= 360; i+=2){
                tft.drawArc(128/2, 128/2, 50, 45, i<=180 ? 180 : 0, i<=180 ? 180 + i : i - 180 , TFT_GREENYELLOW + 0x1111 * color, TFT_BLACK);
                ulTaskNotifyTake(pdTRUE, 1 / portTICK_PERIOD_MS);
                if(TSCA::eventState == TSCA::FOUND_I2C)
                    return;
            }
            color++;
        }
    }

    void resetProgress(TFT_eSPI& tft, char* buf, uint8_t* index){
        *index = 0;
        memset(buf, 'X', PASS_SIZE);
        buf[PASS_SIZE] = '\0'; // Null-terminate the buffer
        drawProggresBar(tft, false, (1 - ((float)(TSCA::NUM_OF_BRUTEFORCE_ITERAIONS) / TSCA::NUM_OF_ITERATIONS)) * 360);
        drawPassDigits(tft, buf, PASS_SIZE, false);
    }

    void drawRatio(TFT_eSPI& tft, uint8_t x, uint8_t y){
        uint8_t size_x = floor(log10(x))+1;
        uint8_t size_y = floor(log10(y))+1;
        uint8_t size = size_x + size_y + 2;
        char* str = new char[size];
        itoa(x,str,10);
        str[size_x] = ':';
        itoa(y,str + size_x + 1,10);
        str[size-1] = '\0';
        log_d("%s", str);
        tft.setTextSize(1);  // Set text size
        tft.drawCentreString(str, textX, textY - textHeight / 2 - 20, 1);
    }


    void TFTTask(void* arg){
        TFT_eSPI tft = TFT_eSPI();
        tft.begin();
        tft.setRotation(6);
        tft.fillScreen(TFT_BLACK);
        
        sleepScrean(tft);

        char buf[PASS_SIZE + 1]; // Create a buffer with an extra space for the null-terminator
        uint8_t index = 0;

        tft.setTextColor(textColor, backgroundColor); // Set text color and background color
        drawRatio(tft, TSCA::NUM_OF_BRUTEFORCE_DIGITS, PASS_SIZE - TSCA::NUM_OF_BRUTEFORCE_DIGITS);
        tft.setTextSize(PASS_SIZE<=6?TextSize:1);  // Set text size
        resetProgress(tft, buf, &index);
        for (;;) {
            // Wait for notification from TSCALoop
            ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
            switch (TSCA::eventState)
            {
            case TSCA::FINISHED_BATCH:
                buf[index] = '0' + TSCA::pass[index];

                drawPassDigits(tft, buf, PASS_SIZE, false);

                index++;
                break;
            case TSCA::SUCCESS:
                for (size_t i = index; i < PASS_SIZE; i++)
                    buf[i] = '0' + TSCA::pass[i];

                drawPassDigits(tft, buf, PASS_SIZE, true);
                
                drawProggresBar(tft, true);
                break;
            case TSCA::FAIL:
                resetProgress(tft, buf, &index);
                break;
            case TSCA::INC:{
                    uint16_t inc = (uint16_t)(TSCA::progress * 3.6);
                    tft.drawArc(128/2, 128/2, 50, 45, inc <=180 ? 180 : 0, inc <=180 ? 180 + inc : inc - 180 , TFT_BLUE, TFT_BLACK);
                    break;
                }
            default:
                break;
            }
            TSCA::eventState = TSCA::NOTHING;
        }

        vTaskDelete(NULL);
    }

    
}

