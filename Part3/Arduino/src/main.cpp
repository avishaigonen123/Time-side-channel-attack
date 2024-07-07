#include <Arduino.h>
#include <SoftwareSerial.h>
#include "shared/EllipticCurve/EllipticCurve.h"

// start the software serial, for communication with the ESP32
SoftwareSerial SerialESP32(6,7);

uint32_t privKey = 0x89;
uint32_t a = 2;
uint32_t b = 3;
uint32_t p = 193939;
EllipticCurve curve(a,b,p);

Point point;
Point res;


void setup(){
    SerialESP32.begin(115200);
    Serial.begin(115200);
}

void loop(){
    if(SerialESP32.available()){
        point.x = SerialESP32.parseInt();
        SerialESP32.read(); // dump the space
        point.y = SerialESP32.parseInt();
		SerialESP32.read(); // dump the newline
        point.print(&Serial);
        // calculate the point, and send via the serial to the server
		Point res = curve.EllipticCurveCalcPoint(point, privKey); 
        res.print(&SerialESP32);
		res.print(&Serial);
        // dump the rest
        byte* garbeg = new byte[SerialESP32.available()];
        SerialESP32.readBytes(garbeg, SerialESP32.available());
        delete[] garbeg;
        garbeg = nullptr;
    }

}
