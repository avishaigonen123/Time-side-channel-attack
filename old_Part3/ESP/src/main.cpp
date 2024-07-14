#include <Arduino.h>
#include <math.h>
#include "shared/EllipticCurve/Point.h"


void setup(){
    Serial2.begin(115200);
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
        point.print(&Serial2);

        byte* garbeg = new byte[Serial2.available()];
        Serial2.readBytes(garbeg, Serial2.available());
        delete[] garbeg;

        // check how much time it takes to calculate the new point
        uint64_t lastTime = millis();
        while(!Serial2.available());
        // send the point to the arduino
        lastTime = millis() - lastTime;

        // print to serial the time it took to calculate the new point
        /*point.x = Serial2.parseInt();
        Serial2.read(); // dump the space
        point.y = Serial2.parseInt();
		Serial2.read(); // dump the newline
        point.print(&Serial);*/
        
        Serial.printf("%" PRIu64 "\n", lastTime);


        // cleanup for next iteration
        garbeg = new byte[Serial2.available()];
        Serial2.readBytes(garbeg, Serial2.available());
        delete[] garbeg;

        garbeg = new byte[Serial.available()];
        Serial.readBytes(garbeg, Serial.available());
        delete[] garbeg;
        garbeg = nullptr;
    }
}
