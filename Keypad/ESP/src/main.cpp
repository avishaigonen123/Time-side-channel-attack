#include <Arduino.h>
#include <pins_arduino.h>
#include <Wire.h>

#define FAIL_PIN 27
#define SUCCESS_PIN 14
#define PASS_SIZE 4

#define ROW_0 GPIO_NUM_15
#define ROW_1 GPIO_NUM_2
#define ROW_2 GPIO_NUM_4
#define ROW_3 GPIO_NUM_16
#define COL_0 GPIO_NUM_17
#define COL_1 GPIO_NUM_5
#define COL_2 GPIO_NUM_18

#define RESET_BUTTON 10
#define ENTER_BUTTON 11
#define TOUCH_DELAY 5


uint8_t rowMap(uint8_t num){
    switch (num)
    {
    case 1:
    case 2:
    case 3:
        return ROW_0;
    case 4:
    case 5:
    case 6:
        return ROW_1;
    case 7:
    case 8:
    case 9:
        return ROW_2;
    case RESET_BUTTON:
    case 0:
    case ENTER_BUTTON:
        return ROW_3;
    default:
        return -1;
    }
}

uint8_t colMap(uint8_t num){
    switch (num)
    {
    case 1:
    case 4:
    case 7:
    case RESET_BUTTON:
        return COL_0;
    case 2:
    case 5:
    case 8:
    case 0:
        return COL_1;
    case 3:
    case 6:
    case 9:
    case ENTER_BUTTON:
        return COL_2;
    default:
        return -1;
    }
}

void writeCol(uint8_t col, uint8_t value) {
    digitalWrite(COL_0, !value);
    digitalWrite(COL_1, !value);
    digitalWrite(COL_2, !value);
    digitalWrite(col, value);
}


void setColLow(){
    digitalWrite(COL_0, LOW);
    digitalWrite(COL_1, LOW);
    digitalWrite(COL_2, LOW);
}

void waitToRow(uint8_t row){
    while(!digitalRead(row));
}

void relaese(){
    setColLow();
	delay(TOUCH_DELAY);
}

bool readyToStart(){
    return !digitalRead(ROW_0) && !digitalRead(ROW_1) && !digitalRead(ROW_2) && !digitalRead(ROW_3);
}

void touch(uint8_t num){

	while(readyToStart()); // wait until ready to start, all raws are zero
    Serial.println("starting touch");
    writeCol(colMap(num), LOW);
    Serial.println("wrote LOW");
    /// set all columns to zero

    setColLow();
    
    waitToRow(rowMap(num));
    Serial.println("got row");

    writeCol(colMap(num), HIGH);
    Serial.println("wrote col");

    while(readyToStart()); // wait until ready to start, all raws are zero
}

void reset(){
	
	touch(RESET_BUTTON);
}

void enter(){
	
	touch(ENTER_BUTTON);
}

void setup() {
    Serial.begin(115200);
    pinMode(FAIL_PIN, INPUT);
    pinMode(SUCCESS_PIN, INPUT);

    pinMode(ROW_0, INPUT);
    pinMode(ROW_1, INPUT);
    pinMode(ROW_2, INPUT);
    pinMode(ROW_3, INPUT);
    pinMode(COL_0, OUTPUT);
    pinMode(COL_1, OUTPUT);
    pinMode(COL_2, OUTPUT);
}
void loop(){
    /*touch(1);
    relaese();
    touch(2);
    relaese();
    touch(3);
    relaese();
    touch(4);
    relaese();*/
    Serial.printf("%d %d %d %d\n",digitalRead(ROW_0),digitalRead(ROW_1),digitalRead(ROW_2),digitalRead(ROW_3));
}
/*
uint32_t sendPassword(uint8_t* password)
{
    
    for (uint8_t i = 0; i < PASS_SIZE; i++)
    {
        touch(password[i]);
        relaese();
    }
    enter();
	uint32_t begin_time = micros();
	while(!digitalRead(FAIL_PIN) && !digitalRead(SUCCESS_PIN));
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
*/