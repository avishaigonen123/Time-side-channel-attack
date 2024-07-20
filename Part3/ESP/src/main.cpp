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
bool ThereIsAPubKey = false;

void loop(){
    if (Serial.available()){
        switch (Serial.read())
        {
        // S1000; 
        case 'S':
        {
            uint32_t numOfSig = Serial.parseInt();
            Serial.read(); // for dumping the ;
            for (size_t i = 0; i < numOfSig; i++)
            {
                uint64_t lastTime = 0;
                do
                {
                    while(!SerialArduino.availableForWrite());
                    sendSigRequest(hash);
                    lastTime = millis();
                } while (!wait(SerialArduino, 100) || SerialArduino.peek() == 'I');
                lastTime = millis() - lastTime;

                ECDSA_sig_t sig;
                if (parseSignature(SerialArduino, &sig))
                    printSigReading(sig, lastTime, Serial);
                else{
                    flush(SerialArduino);
                    log_e("Failed to parseSig");
                }
            }
            Serial.write(';');
            break;
        }
        case 'P':
            sendPubRequest();
            if (!ThereIsAPubKey){
                if (parsePubKey(SerialArduino, &PubKey)){
                    printPubKey(PubKey, Serial);
                    Serial.write(';');
                } else {
                    flush(SerialArduino);
                }
            }
            else
                printPubKey(PubKey, Serial);
                Serial.write(';');
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
    while(!SerialArduino.availableForWrite());
    SerialArduino.write('S');
    SerialArduino.print(hash);
    SerialArduino.write(';');
}

void sendPubRequest(){
    while(!SerialArduino.availableForWrite());
    SerialArduino.write('P');
}

// dump garbage
void flush(Stream &serial){
    while(serial.available())
        serial.read();
}