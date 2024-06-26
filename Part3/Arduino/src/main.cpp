#include <Arduino.h>
#include <SoftwareSerial.h>
#include "shared/EllipticCurve/EllipticCurve.h"
#include "shared/PointStream/PointStream.h"

// start the software serial, for communication with the ESP32
SoftwareSerial SerialESP32(6,7);

uint32_t privKey = 49;
uint32_t a = 2;
uint32_t b = 3;
uint32_t p = 97;
EllipticCurve curve(a,b,p);
PointStream pointStream(&SerialESP32);

Point point;
Point res;


void setup(){
    SerialESP32.begin(115200);
    Serial.begin(115200);
	point = {17, 10};
	curve.EllipticCurveCalcPoint(point, privKey).print();
}

void loop(){
    if(SerialESP32.available()){
        if(pointStream.Recive(&point)){
			//point.print();
			res = curve.EllipticCurveCalcPoint(point, privKey);
			//res.print();
			pointStream.send(&res);
			res = {0, 0};
			point = {0, 0};
			byte* garbeg = new byte[SerialESP32.available()];
			SerialESP32.readBytes(garbeg, SerialESP32.available());
			delete[] garbeg;
			garbeg = nullptr;
		}
    }

}