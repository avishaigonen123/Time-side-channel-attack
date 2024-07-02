#include <Arduino.h>

#define RED_LED 12
#define GREEN_LED 13

void setup(){
    pinMode(RED_LED, OUTPUT);
    pinMode(GREEN_LED, OUTPUT);
}

void loop() {
    delay(1000);
    digitalWrite(RED_LED, HIGH);
    delayMicroseconds(1);
    digitalWrite(RED_LED, LOW);
}