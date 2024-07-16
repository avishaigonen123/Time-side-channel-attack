#include <Arduino.h>
#include <math.h>
#include "shared/EllipticCurve/Point.h"

HardwareSerial& SerialArduino = Serial2;

// clean the garbage
void flush(Stream& stream){
    byte* garbeg = new byte[stream.available()];
    stream.readBytes(garbeg, stream.available());
    delete[] garbeg;
    garbeg = nullptr;
}

void sendToMaster(uint32_t r, uint32_t s, uint64_t time)
{
    Serial.printf("%" PRIu32 " %" PRIu32 " %" PRIu64 "\n", r, s, time);
}

void setup(){
    SerialArduino.begin(115200);
    Serial.begin(115200);
}

void loop(){
    if(Serial.available() && Serial.read() == 'S'){
        // it receives ack from the server (PC)

        // send start to the arduino
        uint64_t lastTime = millis();
        
        SerialArduino.write('S');
        
        // cleanup for next iteration
        flush(SerialArduino);
        
        // check how much time it takes to calculate the new point
        while(!SerialArduino.available());
        
        // send the point to the arduino
        lastTime = millis() - lastTime;
        
        uint32_t r = SerialArduino.parseInt();
        SerialArduino.read(); // dump the space
        uint32_t s = SerialArduino.parseInt();
		SerialArduino.read(); // dump the newline

        // send to PC the signature and the time it took to be calculated
        sendToMaster(r, s, lastTime);

        flush(SerialArduino);
        flush(Serial);
    }
}
