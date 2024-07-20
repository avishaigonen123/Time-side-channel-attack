#pragma once
#include "ECDSA.h"

// function that gets timeout, and wait for serial to be aviliable for a period of this timeout
bool inline wait(Stream &serial, unsigned int timeout){
    int t = millis();
    while(!serial.available()){
        if(millis() - t > timeout){
            return false;
        }
    }
    return true;
}

// Sent to Serial the signature
void inline printSignature(ECDSA_sig_t sig, Stream &serial) {
    serial.print("SIG<");
    serial.print(sig.r);
    serial.write(',');
    serial.print(sig.s);
    serial.write('>');
}

// sent to Serial the public key
void inline printPubKey(Point pub, Stream &serial) {
    serial.print("PUB<");
    serial.print(pub.x);
    serial.write(',');
    serial.print(pub.y);
    serial.write('>');
}

// function that gets signature from serial and parse it into other format
// SIG<273,1024>
bool inline parseSignature(Stream & stream, ECDSA_sig_t* sig){
    char prefix[] = "SIG<";
    
    for (uint8_t index = 0; index < sizeof(prefix) - 1; index++) {
        if(!wait(stream, 20))
            return false;
        uint8_t c = stream.read();
        if (c != prefix[index]) {
            Serial.print("Expected: ");
            Serial.println(prefix[index]);
            Serial.print("Read: ");
            Serial.println(c);
            return false;
        }
    }

    ECDSA_sig_t tempSig;
    tempSig.r = stream.parseInt();

    if(!wait(stream, 20))
        return false;

    if (stream.read() != ',')
        return false;
    tempSig.s = stream.parseInt();

    if(!wait(stream, 20))
        return false;

    if (stream.read() != '>')
        return false;
    
    *sig = tempSig;
    return true;
}

// PUB<x,y>
bool inline parsePubKey(Stream & stream, Point* pub){
    char prefix[] = "PUB<";
    
    for (uint8_t index = 0; index < sizeof(prefix) - 1; index++) {
        wait(stream, 20);
        uint8_t c = stream.read();
        if (c != prefix[index]) {
            Serial.print("Expected: ");
            Serial.println(prefix[index]);
            Serial.print("Read: ");
            Serial.println(c);
            return false;
        }
    }
        
    Point tempPub;
    tempPub.x = stream.parseInt();

    wait(stream, 20);

    if (stream.read() != ',')
        return false;

    tempPub.y = stream.parseInt();

    wait(stream, 20);

    if (stream.read() != '>')
        return false;

    *pub = tempPub;
    return true;
}

// VER<SIG<r,s>,hash>
bool inline parseVerify(Stream &stream, ECDSA_sig_t* sig, uint32_t* hash) {
    const char prefix[] = "VER<";
    for (uint8_t index = 0; index < sizeof(prefix) - 1; index++) {
        wait(stream, 20);
        uint8_t c = stream.read();
        if (c != prefix[index]) {
            // printing for debug
            Serial.print("Expected: ");
            Serial.println(prefix[index]);
            Serial.print("Read: ");
            Serial.println(c);
            return false;
        }
    }
    
    ECDSA_sig_t _sig;
    if (!parseSignature(stream, &_sig)) {
        stream.println("Failed to parse signature");
        return false;
    }

    wait(stream, 20);

    if (stream.read() != ',') {
        stream.println("Expected comma");
        return false;
    }

    uint32_t _hash = stream.parseInt();
    
    wait(stream, 20);
    
    if (stream.read() != '>') {
        stream.println("Expected closing bracket");
        return false;
    }
    *sig = _sig;
    *hash = _hash;
    return true;
}

