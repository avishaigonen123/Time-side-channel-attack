#include "ECDSA.h"
#include <Arduino.h>
#include "EllipticCurve.h"
#include "Math.h"

// Debugging helper to print points
void printPoint(Point p) {
    Serial.print("Point: (");
    Serial.print(p.x);
    Serial.print(", ");
    Serial.print(p.y);
    Serial.println(")");
}

// Debugging helper to print signature
void printSignature(ECDSA_sig_t sig) {
    Serial.print("Signature: (r=");
    Serial.print(sig.r);
    Serial.print(", s=");
    Serial.print(sig.s);
    Serial.println(")");
}

ECDSA::ECDSA() {
    needsToBeDeleted = true;
    curve = new EllipticCurve(16, 20, 991);
    G = Point(589, 52);
    n = curve->calcOrder(G);
}

ECDSA::ECDSA(EllipticCurve *_curve, Point _G) {
    curve = _curve;
    G = _G;
    n = _curve->calcOrder(_G);
}

ECDSA::~ECDSA() {
    if (needsToBeDeleted) {
        delete curve;
        curve = nullptr;
    }
}

ECDSA_keypair_t ECDSA::make_keypair() {

    randomSeed(analogRead(0));
    return make_keypair(random(1, n-1));
}

ECDSA_keypair_t ECDSA::make_keypair(uint32_t priv)
{
    ECDSA_keypair_t keys;
    keys.privKey = priv;
    keys.Q = curve->scalarMultiplication(G, keys.privKey);
    return keys;
}

ECDSA_sig_t ECDSA::sign(uint32_t privKey, uint8_t *message, uint32_t len) {
    return sign(privKey, hash(message, len));
}

ECDSA_sig_t ECDSA::sign(uint32_t privKey, uint32_t hash) {
    randomSeed(analogRead(0));
    ECDSA_sig_t sig;
    do {
        uint32_t k = random(1, n-1); // nonce
        sig.r = (curve->scalarMultiplication(G, k)).x % n;
        sig.s = (modularInverse(k, n)  * (hash + privKey * sig.r)) % n;

        // Serial.print("k: ");
        // Serial.println(k);
        // printSignature(sig);

    } while (sig.s == 0 || sig.r == 0 || modularInverse(sig.s, n) == -1);
    
    return sig;
}

uint32_t ECDSA::hash(uint8_t* mes, uint32_t len) {
    uint32_t res = 0;
    for (size_t i = 0; i < len; i++)
        res = (res + mes[i]) * 31;
    return res;
}

bool ECDSA::verify(Point Q, ECDSA_sig_t sig, uint32_t hash) {
    if(!curve->isOnCurve(Q))
        return false; // if Q is not on curve -> false
    uint32_t w = modularInverse(sig.s, n) % n;
    uint32_t u1 = (hash * w) % n;
    uint32_t u2 = (sig.r * w) % n;
    Point temp1 = curve->scalarMultiplication(G, u1);
    Point temp2 = curve->scalarMultiplication(Q, u2);
    Point R = curve->addPoint(temp1, temp2);
    uint32_t rPrime = R.x % n;

    return rPrime == sig.r;
}

bool ECDSA::verify(Point Q, ECDSA_sig_t sig, uint8_t *message, uint32_t len)
{
    return verify(Q, sig, hash(message, len));
}
