#include "ECDSA.h"
#include <Arduino.h>
#include "EllipticCurve.h"
#include "Math.h"

/**
 * a hash function for Poc, can be replaced virtualy with any other hash function
 */
uint32_t ECDSA::hash(uint8_t* mes, uint32_t len)
{
    uint32_t res = 0;
    for (size_t i = 0; i < len; i++)
        res = (res + mes[i]) * 31;
    return res;
}

ECDSA::ECDSA()
{
    needsToBeDeleted = true;
    curve = new EllipticCurve(2, 3, 97);
    G = Point(83, 23);
    n = curve->calcOrder(G);
}

ECDSA::ECDSA(EllipticCurve *_curve, Point _G)
{
    curve = _curve;
    G = _G;
    n = _curve->calcOrder(_G);
}

ECDSA::~ECDSA()
{
    if(needsToBeDeleted){
        delete curve;
        curve = nullptr;
    }
}

ECDSA_keypair_t ECDSA::make_keypair()
{
    
    ECDSA_keypair_t keys;
    randomSeed(analogRead(0));
    keys.privKey = random(1, n-1);
    keys.Q = curve->scalarMultiplication(G, keys.privKey);

    return keys;
}

ECDSA_sig_t ECDSA::sign(uint32_t privKey, uint8_t *message, uint32_t len)
{
    return sign(privKey, hash(message, len));
}

ECDSA_sig_t ECDSA::sign(uint32_t privKey, uint32_t hash)
{
    randomSeed(analogRead(0));
    uint32_t k = random(1, n-1);
    ECDSA_sig_t sig;
    sig.r = (curve->scalarMultiplication(G, k)).x;
    sig.s = (modularInverse(k, n) * (hash + privKey * sig.r)) % n;
    return sig;
}

bool ECDSA::verify(Point Q, ECDSA_sig_t sig, uint32_t hash)
{
    uint32_t s1 = modularInverse(sig.s, n) % n;
    Point temp1 = curve->scalarMultiplication(G, (hash * s1) % n);
    Point temp2 = curve->scalarMultiplication(Q, (sig.r * s1) % n);
    uint32_t rPrime = curve->addPoint(temp1, temp2).x ;
    return rPrime == sig.r;
}
