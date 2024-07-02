#include <Arduino.h>

#define FAIL_PIN 27
#define FAIL_PIN2 12

hw_timer_t * timer = NULL;
volatile uint64_t startTime = 0;
volatile uint64_t endTime = 0;
volatile bool measure = false;

// ISR for FAIL_PIN
void IRAM_ATTR stopTimer() {
    if (measure) {
        endTime = timerRead(timer);
        measure = false;
    }
}

// ISR for FAIL_PIN2
void IRAM_ATTR startTimer() {
    if (!measure) {
        timerWrite(timer, 0); // Reset the timer
        startTime = timerRead(timer);
        measure = true;
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(FAIL_PIN, INPUT);
    pinMode(FAIL_PIN2, INPUT);
    
    // Attach interrupts
    attachInterrupt(digitalPinToInterrupt(FAIL_PIN), stopTimer, FALLING);
    attachInterrupt(digitalPinToInterrupt(FAIL_PIN2), startTimer, RISING);
    
    // Initialize the timer
    timer = timerBegin(0, 2, true); // Use timer 0, prescaler 1 (4.17ns per tick), count up
}

void loop() {
    if (!measure && startTime != 0 && endTime != 0) {
        uint64_t duration = endTime - startTime;
        Serial.print("Time in nanoseconds: ");
        Serial.println(duration * 4.17 * 2, 2); // Print the time in nanoseconds with 2 decimal places
        startTime = 0; // Reset startTime for next measurement
        endTime = 0; // Reset endTime for next measurement
    }
}
