#include <Arduino.h>
#include <math.h>
#include "shared/EllipticCurve/Point.h"

void setup(){
    Serial2.begin(115200);
    Serial.begin(115200);
}

void loop(){
    if(Serial.available()){
        Point point;
        point.x = Serial.parseInt();
        Serial.read(); // dump the space
        point.y = Serial.parseInt();
		Serial.read(); // dump the newline

        point.print(&Serial2);

        uint64_t lastTime = millis();
        while(!Serial2.available());
        lastTime = millis() - lastTime;

        Serial.printf("%" PRIu64 "\n", lastTime);

        // cleanup for next iteration
        
        byte* garbeg = new byte[Serial2.available()];
        Serial2.readBytes(garbeg, Serial2.available());
        delete[] garbeg;

        garbeg = new byte[Serial.available()];
        Serial.readBytes(garbeg, Serial.available());
        delete[] garbeg;
        garbeg = nullptr;
    }
}
