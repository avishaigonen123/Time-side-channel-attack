#include <Arduino.h>
#include <math.h>
#include "shared/EllipticCurve/Point.h"
#include "shared/PointStream/PointStream.h"

PointStream pointStream(&Serial2);

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
        bool good = false;
        uint64_t lastTime;
        Point res;
        while(!good){
            pointStream.send(&point);
            lastTime = micros();
            while(!Serial2.available());
            lastTime = micros() - lastTime;
            res = {0, 0};
            good = pointStream.Recive(&res) && lastTime > 20;
        }

        Serial.printf("%" PRIu64 "\n", lastTime);
        // dump the rest
        /*byte* garbeg = new byte[Serial.available()];
        Serial.readBytes(garbeg, Serial.available());
        delete[] garbeg;
        garbeg = nullptr;*/
    }
}
