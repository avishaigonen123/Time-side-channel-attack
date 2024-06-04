#include <Arduino.h>
#include <pins_arduino.h>
#include <Wire.h>

#define FAIL_PIN 27
#define SUCCESS_PIN 14
#define PASS_SIZE 4

#define RESET_BUTTON 0x001
#define ENTER_BUTTON 0x100
#define INITIALIZE_VALUE 0x909
#define TOUCH_DELAY 6

const uint8_t real_to_fake_keypad[10] = {4, 3, 7, 11, 2, 6, 10, 1, 5 ,9};

volatile uint8_t data[4];

uint16_t state = 0;


void relaese(){
	state = 0;
	delay(TOUCH_DELAY);
}

void touch(uint8_t num){

	state = 1 << real_to_fake_keypad[num];
	delay(TOUCH_DELAY);
}

void reset(){
	
	state = RESET_BUTTON;
	delay(TOUCH_DELAY);
}

void enter(){
	
	state = ENTER_BUTTON;
}

void onRec_Callback(int numOfBytes){
    for(uint8_t i = 0 ; Wire.available() ; i++)
    {
        data[i] = Wire.read();
        //Serial.print(, HEX);
        //Serial.print(" ");
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
    }
    //Serial.println("\n++++++++++++++++++++++++++++++++++++");
    data[0] = 0xFF;
}

void setup() {
    Serial.begin(115200);
    
    for (uint8_t i = 0; i < 4; i++)
    {
        data[i] = 0xFF;
    }
    interrupts();
    Wire.begin(0x5A, SDA, SCL, 400000);
    Wire.onReceive(onRec_Callback);
    Wire.onRequest(onReq_Callback);
    pinMode(FAIL_PIN, INPUT);
    pinMode(SUCCESS_PIN, INPUT);
	Serial.println(clockCyclesPerMicrosecond());
	relaese();
	delay(1000);
}

uint32_t sendPassword(uint8_t* password)
{
    
    for (uint8_t i = 0; i < PASS_SIZE; i++)
    {
        touch(password[i]);
        relaese();
    }
    enter();
	uint32_t begin_time = micros();
	noInterrupts();
	while(!digitalRead(FAIL_PIN) && !digitalRead(SUCCESS_PIN));
	interrupts();
	uint32_t end_time = micros();

    return  end_time - begin_time;
}
uint8_t pass[PASS_SIZE] = {0,0,0,0};
void loop() {
    // generate the password each loop iteration
	
	uint32_t res;
    uint8_t maxes[10] = {0,0,0,0,0,0,0,0,0,0};
    uint32_t results[10] = {0,0,0,0,0,0,0,0,0,0};
	for (uint8_t i = 0; i < 50; i++){
		uint32_t max_time = 0;
		for (uint8_t j = 0; j < 10; j++)
		{
			pass[0] = j;
            results[j] = sendPassword(pass);
            if(results[j] > max_time)
                max_time = results[j];
         
		}
        for (uint8_t j = 0; j < 10; j++)
            if(results[j] == max_time)
                maxes[j]++;
		
	} 
    for(int i = 0; i < 10; i++)
    {
        Serial.print(maxes[i]);
        Serial.print(" ");
    }
	Serial.println("-----------------------------");
}
