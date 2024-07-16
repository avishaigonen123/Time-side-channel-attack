#pragma once
#include "ECDSA.h"

// Debugging helper to print signature
void inline printSignature(ECDSA_sig_t sig, Stream &serial) {
    serial.print("SIG<");
    serial.print(sig.r);
    serial.write(',');
    serial.print(sig.s);
    serial.write('>');
}

void inline printPubKey(Point pub, Stream &serial) {
    serial.print("PUB<");
    serial.print(pub.x);
    serial.write(',');
    serial.print(pub.y);
    serial.write('>');
}

// <SIG|273|1024>
bool inline parseSignature(Stream & stream, ECDSA_sig_t* sig){
    char prefix[] = "SIG<";
    
    for (uint8_t index = 0; index < sizeof(prefix) - 1; index++) {
        while(!Serial.available());
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

    while(!Serial.available());

    if (stream.read() != ',')
        return false;
    tempSig.s = stream.parseInt();

    while(!Serial.available());

    if (stream.read() != '>')
        return false;
    
    *sig = tempSig;
    return true;
}

bool inline parsePubKey(Stream & stream, Point* pub){
    char prefix[] = "PUB<";
    
    for (uint8_t index = 0; index < sizeof(prefix) - 1; index++) {
        while(!Serial.available());
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

    while(!Serial.available());

    if (stream.read() != ',')
        return false;

    tempPub.y = stream.parseInt();

    while(!Serial.available());

    if (stream.read() != '>')
        return false;

    *pub = tempPub;
    return true;
}

bool inline parseVerify(Stream &stream, ECDSA_sig_t* sig, uint32_t* hash) {
    const char prefix[] = "VER<";
    for (uint8_t index = 0; index < sizeof(prefix) - 1; index++) {
        while(!Serial.available());
        uint8_t c = stream.read();
        if (c != prefix[index]) {
            Serial.print("Expected: ");
            Serial.println(prefix[index]);
            Serial.print("Read: ");
            Serial.println(c);
            return false;
        }
    }
    
    ECDSA_sig_t _sig;
    if (!parseSignature(stream, &_sig)) {
        Serial.println("Failed to parse signature");
        return false;
    }

    while(!Serial.available());

    if (stream.read() != ',') {
        Serial.println("Expected comma");
        return false;
    }

    while(!Serial.available());
    
    uint32_t _hash = stream.parseInt();
    if (stream.read() != '>') {
        Serial.println("Expected closing bracket");
        return false;
    }
    *sig = _sig;
    *hash = _hash;
    return true;
}

