#pragma once

#include <Arduino.h>
#include "EllipticCurve.h"

typedef struct ECDSA_sig_t{
    uint32_t r;
    uint32_t s;
} ECDSA_sig_t;

typedef struct ECDSA_keypair_t{
    uint32_t privKey;
    Point Q;
} ECDSA_keypair_t;

class ECDSA{
    // the curve
    EllipticCurve* curve;

    // Generator Point
    Point G;
    // the group order
    uint32_t n;

    // this flag is being set if the instance used new, NOT relevent to the attack
    bool needsToBeDeleted = false;

    // the hash function used
    uint32_t hash(uint8_t* mes, uint32_t len);
public:
    ECDSA();
    ECDSA(EllipticCurve* curve, Point G);
    ~ECDSA();
    ECDSA_keypair_t make_keypair();
    ECDSA_keypair_t make_keypair(uint32_t priv);
    ECDSA_sig_t sign(uint32_t privKey, uint8_t* message, uint32_t len);
    ECDSA_sig_t sign(uint32_t privKey, uint32_t hash);
    bool verify(Point Q, ECDSA_sig_t sig, uint8_t* message, uint32_t len);
    bool verify(Point Q, ECDSA_sig_t sig, uint32_t hash);
};