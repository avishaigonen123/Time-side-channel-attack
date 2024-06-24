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
        point.print();
        bool good = false;
        uint64_t lastTime;
        Point res;
        while(!good){
            pointStream.send(&point);
            lastTime = micros();
            while(!Serial2.available());
            lastTime = micros() - lastTime;
            res = {0, 0};
            good = pointStream.Recive(&res) && lastTime > 200;
        }

        Serial.printf("%" PRIu64 ",%" PRId32 ",%" PRId32 "\n", lastTime, res.x, res.y);
        while(Serial.available()) Serial.read(); // dump the rest
    }
}
