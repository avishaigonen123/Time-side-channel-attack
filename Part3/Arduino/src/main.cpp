#include <Arduino.h>
#include <SoftwareSerial.h>
#include "EllipticCurve/EllipticCurve.h"

// start the software serial, for communication with the ESP32
SoftwareSerial SerialESP32(6,7);

uint32_t privKey = 49;
uint32_t a = 5;
uint32_t b = 2;
uint32_t p = 97;
EllipticCurve curve(a,b,p);

Point point;
char buffer[sizeof(Point)];

void setup(){
    SerialESP32.begin(115200);
    Serial.begin(115200);
}

void loop(){
    if(SerialESP32.available())
    {
        // get point from client
        SerialESP32.readBytes(buffer, sizeof(Point));   
        memcpy(&point, buffer, sizeof(Point));
        
        // send point after algorithm
        //point = curve.EllipticCurveCalcPoint(point, privKey);
        SerialESP32.write((uint8_t*)&point, sizeof(Point));
    }
}