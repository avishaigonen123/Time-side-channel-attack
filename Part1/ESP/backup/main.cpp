#include <Arduino.h>
#include <pins_arduino.h>
#include <Wire.h> 
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "touchHandle.h"

using namespace Touch;

#define FAIL_PIN GPIO_NUM_27
#define FAIL_PIN2 12
#define SUCCESS_PIN 14

#define PASS_SIZE 4 

hw_timer_t * timer = NULL;
volatile uint64_t endTime = 0;
volatile bool measure = false;

void IRAM_ATTR stopTimer();

void IRAM_ATTR startTimer();

void onRec_Callback(int numOfBytes);

void onReq_Callback();

uint64_t sendPassword(uint8_t* password);

void MainLoop(void*);

void i2cTask(void *parameter);

void theEnd();

//================================================================================================

void setup() {
    Serial.begin(115200);
    
	xTaskCreatePinnedToCore(
		MainLoop,
        "main",
        10000,
        NULL,
        100,
        NULL,
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


void loop(){Serial.println("wont Print?");}


void i2cTask(void *parameter) {
    Wire.begin(0x5A, SDA, SCL, 400000);
	
    Wire.onReceive(onRec_Callback);
    Wire.onRequest(onReq_Callback);
    vTaskDelete(NULL);
}

uint8_t pass[PASS_SIZE];

void MainLoop(void*) {

	pinMode(FAIL_PIN, INPUT);
    pinMode(SUCCESS_PIN, INPUT);
    
    // Attach interrupts
    attachInterrupt(digitalPinToInterrupt(FAIL_PIN), stopTimer, RISING);
    attachInterrupt(digitalPinToInterrupt(SUCCESS_PIN), theEnd, RISING);

    delay(10);
    TaskHandle_t i2c_th = xTaskGetHandle("i2c_slave_task");
    if (i2c_th != NULL) {
        // Task handle is valid, proceed with notification
        //xTaskNotify(i2c_th, 0x01, eSetBits);
    } else {
        // Task handle is invalid, handle error or debug
        Serial.println("Invalid task handle!");
    }

    // Initialize the timer
    timer = timerBegin(0, 2, true); // Use timer 0, prescaler 1 (4.17ns per tick), count up

	for (uint8_t i = 0; i < 4; i++)
    {
        data[i] = 0xFF;
    }

    delay(1000);
	reset();
	relaese();
	delay(4000);
    
    const uint8_t initPass[PASS_SIZE] = {0,0,0,0};
    // generate the password each loop iteration
	for(;;){
        memcpy(pass, initPass, PASS_SIZE);

		for (uint8_t index = 0; index < PASS_SIZE; index++)
		{
			uint8_t maxes[10] = {0,0,0,0,0,0,0,0,0,0};
			uint32_t results[10] = {0,0,0,0,0,0,0,0,0,0};
			for (uint8_t i = 0; i < 20; i++){
				uint32_t max_time = 0;
				for (uint8_t j = 0; j < 10; j++)
				{
					pass[index] = j;
					results[j] = sendPassword(pass);
					if(results[j] > max_time)
						max_time = results[j];
				}
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
}

//================================================================================================

// ISR for FAIL_PIN
void IRAM_ATTR stopTimer() {
    if (measure) {
        endTime = timerRead(timer);
        measure = false;
    }
}

void IRAM_ATTR startTimer() {
    if (!measure) {
        timerWrite(timer, 0); // Reset the timer
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
        //Serial.println("sent 0x24");
    }
    else if(data[0] == 0x5D){
        Wire.write(0x0);
        //Serial.println("sent 0x0");
    }
    else if(data[0] == 0x00){
        Wire.write((uint8_t)state);
        Wire.write(state >> 8);
		if(state == ENTER_BUTTON){
			startTimer();
		}
    }
    //Serial.println("\n++++++++++++++++++++++++++++++++++++");
    data[0] = 0xFF;
}

uint64_t sendPassword(uint8_t* password)
{
    
    for (uint8_t i = 0; i < PASS_SIZE; i++)
    {
        touch(password[i]);
        relaese();
    }

    enter();
	while (measure || endTime == 0){}
		//Serial.printf("%d ,%" PRIu64 ",%" PRIu64 "\n" , measure, endTime, startTime);

    uint64_t duration = endTime;
    //startTime = 0; // Reset startTime for next measurement
    endTime = 0; // Reset endTime for next measurement

    return duration;
}

void theEnd(){
    if(state == ENTER_BUTTON){
        Serial.println("The End");
        for (size_t i = 0; i < PASS_SIZE; i++){
            Serial.print(pass[i]);
        }
    }
}