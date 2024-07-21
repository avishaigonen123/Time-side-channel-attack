#pragma once
#include <Arduino.h>

namespace StopWatch{
    hw_timer_t * fastTimer = NULL;
    volatile uint64_t endTime = 0;
    volatile bool measure = false;
    
    void init(){
        fastTimer = timerBegin(0, 2, true); // Use timer 0, prescaler 1 (4.17ns per tick), count up
    }

    // ISR for FAIL_PIN
    void IRAM_ATTR stopTimer() {
        if (measure) {
            endTime = timerRead(fastTimer);
            measure = false;
        }
    }

    void IRAM_ATTR startTimer() {
        if (!measure) {
            timerWrite(fastTimer, 0); // Reset the timer
            measure = true;
        }
    }

    uint64_t wait(){
        while (measure || endTime == 0){}
            //Serial.printf("%d ,%" PRIu64 ",%" PRIu64 "\n" , measure, endTime, startTime);
        uint64_t duration = endTime;
        endTime = 0; // Reset endTime for next measurement
        return duration;
    }
}