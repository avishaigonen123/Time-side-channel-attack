#include <Arduino.h>
#include <math.h>
#include "shared/EllipticCurve/Point.h"


void setup(){
    SerialArduino.begin(115200);
    Serial.begin(115200);
}

void loop(){
    if(Serial.available()){
        // it receives points from the server (PC)
        Point point;
        point.x = Serial.parseInt();
        Serial.read(); // dump the space
        point.y = Serial.parseInt();
		Serial.read(); // dump the newline

        // send the point to the arduino
        point.print(&SerialArduino);

        // check how much time it takes to calculate the new point
        uint64_t lastTime = millis();
        while(!SerialArduino.available());
        lastTime = millis() - lastTime;

        // print to serial the time it took to calculate the new point
        Serial.printf("%" PRIu64 "\n", lastTime);

        // cleanup for next iteration
        byte* garbeg = new byte[SerialArduino.available()];
        SerialArduino.readBytes(garbeg, SerialArduino.available());
        delete[] garbeg;

        garbeg = new byte[Serial.available()];
        Serial.readBytes(garbeg, Serial.available());
        delete[] garbeg;
        garbeg = nullptr;
    }
}
