#include <Arduino.h>
#include <math.h>

typedef struct Point{
    int32_t x, y;
} Point;

Point point;
Point res;

void cb(){
    uint8_t buff[sizeof(Point)];
    Serial2.read(buff, sizeof(Point));
    memcpy(&res, buff, sizeof(Point));
}

void setup(){
    Serial2.begin(115200);
    Serial.begin(115200);
    point = {
        .x = 24,
        .y = 47
    };
    Serial2.onReceive(cb);
}

void loop(){
    if(Serial2.availableForWrite()){
        Serial2.write((uint8_t*)&point, sizeof(Point));
        delay(100);
        Serial.printf("%d, %d\n", res.x, res.y);
    }
}
