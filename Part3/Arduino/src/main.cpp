#include <Arduino.h>
#include <SoftwareSerial.h>
#include "shared/EllipticCurve/ECDSA.h"

// start the software serial, for communication with the ESP32
SoftwareSerial SerialESP32(6,7);

ECDSA ecdsa;
uint32_t privKey = 0xBA;

uint32_t hash = 1234;

// Debugging helper to print signature
void printSignature(ECDSA_sig_t sig, Stream &serial) {
    serial.print(sig.r);    
    serial.print(" ");
    serial.println(sig.s);
}

// dump garbage
void flush(Stream &serial){
    byte* garbeg = new byte[serial.available()];
    serial.readBytes(garbeg, serial.available());
    delete[] garbeg;
    garbeg = nullptr;
}

// start serials
void setup(){
    SerialESP32.begin(115200);
    Serial.begin(115200);
}

void loop(){
    if(SerialESP32.available() && SerialESP32.read() == 'S'){

        ECDSA_sig_t sig = ecdsa.sign(privKey, hash);
        printSignature(sig, SerialESP32); 
        printSignature(sig, Serial);

        // printSignature(sig, Serial);
        // dump the rest
        flush(SerialESP32);
        // flush(Serial);
    }
}

// ECDSA ecdsa;
// uint32_t hash = 1234;

// void setup(){
//     Serial.begin(115200);
//     randomSeed(analogRead(0));
//     ECDSA_keypair_t keys = ecdsa.make_keypair();

//     for (size_t i = 0; i < 20; i++){
//         ECDSA_sig_t sig = ecdsa.sign(keys.privKey, hash);
//         Serial.println(ecdsa.verify(keys.Q, sig, hash));
//     }
// }

// void loop(){

    
// }

