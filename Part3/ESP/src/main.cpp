#include <Arduino.h>
#include <math.h>
#include "shared/EllipticCurve/ECDSA.h"
#include "shared/EllipticCurve/Protocol.h"

HardwareSerial& SerialArduino = Serial2;

void sendSig(uint32_t r, uint32_t s, uint64_t time);
void sendSigRequest(uint32_t hash);
void printSigReading(ECDSA_sig_t sig, uint64_t time, Stream &serial);
void sendPubRequest();
void flush(Stream &serial);

uint32_t hash = 1234;

void setup(){
    SerialArduino.begin(115200);
    Serial.begin(115200);
}

Point PubKey;
bool isThrePubKey = false;

void loop(){
    if (Serial.available()){
        switch (Serial.read())
        {
        case 'S':
        {
            sendSigRequest(hash);
            
            uint64_t lastTime = millis();
            while(!SerialArduino.available());
            lastTime = millis() - lastTime;

            ECDSA_sig_t sig;
            if (parseSignature(SerialArduino, &sig))
                printSigReading(sig, lastTime, Serial);
            else{
                flush(SerialArduino);
            }
            break;
        }
        case 'P':
            sendPubRequest();

            while(!SerialArduino.available());
            if (!isThrePubKey){
                if (parsePubKey(SerialArduino, &PubKey))
                    printPubKey(PubKey, Serial);
                else{
                    flush(SerialArduino);
                }
            }
            else
                printPubKey(PubKey, Serial);
            break;
        default:
            break;
        }
    }
}

void printSigReading(ECDSA_sig_t sig, uint64_t time, Stream &serial) {
    serial.print("SIG<");
    serial.print(sig.r);
    serial.write(',');
    serial.print(sig.s);
    serial.write(',');
    serial.print(time);
    serial.write('>');
}

void sendSigRequest(uint32_t hash){
    SerialArduino.write('S');
    SerialArduino.print(hash);
    SerialArduino.write(';');
}

void sendPubRequest(){
    SerialArduino.write('P');
}

// dump garbage
void flush(Stream &serial){
    while(serial.available())
        serial.read();
}