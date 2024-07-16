#include <Arduino.h>
#include <SoftwareSerial.h>
#include "shared/EllipticCurve/ECDSA.h"
#include "shared/EllipticCurve/Protocol.h"

// start the software serial, for communication with the ESP32
SoftwareSerial SerialESP32(6,7);
ECDSA ecdsa;

void flush(Stream &serial);

uint32_t privKey = 0x69;
ECDSA_keypair_t keys;

void setup(){
    Serial.begin(115200);
    SerialESP32.begin(115200);
    keys = ecdsa.make_keypair(privKey);
}

void loop(){
    if (SerialESP32.available()){
        switch (SerialESP32.peek())
        {
        case 'S':
            {
                SerialESP32.read();
                uint32_t hash = SerialESP32.parseInt();
                if (hash){
                    printSignature(ecdsa.sign(keys.privKey, hash), SerialESP32);
                } else {
                    SerialESP32.print("I");
                }
                break;
            }
        case 'P':
            SerialESP32.read();
            printPubKey(keys.Q, SerialESP32);
            break;
        case 'V':
        {
            uint32_t hash;
            ECDSA_sig_t sig;
            if (parseVerify(SerialESP32, &sig, &hash)){
                if (ecdsa.verify(keys.Q, sig, hash))
                    SerialESP32.println("OK");
                else
                    SerialESP32.println("INVALID SIG");
            }
            else{
                flush(SerialESP32);
                SerialESP32.println("ERROR IN SIG");
            }
            break;
        }
        default:
            SerialESP32.read();
            break;
        }
    } 
}

// dump garbage
void flush(Stream &serial){
    while(serial.available())
        serial.read();
}