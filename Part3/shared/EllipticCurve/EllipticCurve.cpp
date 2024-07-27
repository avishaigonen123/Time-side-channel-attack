#include <Arduino.h>
#include "EllipticCurve.h"
#include "Math.h"

// Function that performs point adding
Point EllipticCurve::addPoint(const Point& point1, const Point& point2)
{
    if (point1.x == point2.x && point1.y == point2.y) // the same point
        return doublingPoint(point1);  
    if (point1.x == point2.x) // next point will be infinity point
        return InfPoint;    
    if (point1 == InfPoint) // return the second point
        return point2;
    if (point2 == InfPoint) // return the second point
        return point1;
    Point R;
    int32_t numerator = point1.y - point2.y;
    int32_t denominator = point1.x - point2.x;

    if (denominator < 0){
        numerator *= -1;
        denominator *= -1;
    }

    int32_t s = module(numerator, p) * modularInverse(denominator, p) % p;

    R.x = s * s - (point1.x + point2.x);
    R.x = module(R.x, p);
    R.y = s * (point1.x - R.x) - point1.y;
    R.y = module(R.y, p);
    return R;
}

// Function that performs point doubling
Point EllipticCurve::doublingPoint(const Point& point){
    if (point == InfPoint) // InfPoint+InfPoint = InfPoint
        return InfPoint;
    if (point.y == 0) // InfPoint because this is a vertical line
        return InfPoint;
    Point R;
    int32_t numerator = 3 * point.x * point.x + a;
    int32_t denominator = 2 * point.y;
    int32_t s = module(numerator, p) * modularInverse(denominator, p) % p;

    R.x = s * s - 2 * point.x;
    R.x = module(R.x, p);
    R.y = s * (point.x - R.x) - point.y;
    R.y = module(R.y, p);
    return R;
}

// Function that implements montgomery algorithm
Point EllipticCurve::scalarMultiplication(Point P, uint32_t k){
    Point R0 = P;
    byte l = key_length(k);
    for(int8_t j = l-2; j >= 0; j--)
    {
        R0 = doublingPoint(R0);
        if(bitRead(k, j)){ // if k[j] == 1
            R0 = addPoint(R0, P);
        }
        delay(5);
    }
    return R0;
}

// Function to calculate the point on the elliptic curve given the private key
Point EllipticCurve::EllipticCurveCalcPoint(Point P, uint32_t PrivKey){
    return scalarMultiplication(P, PrivKey); // use the scalar multiplication algorithm
}

// this function calculates the length in bits of the key
byte EllipticCurve::key_length(uint32_t k)
{
    for (uint8_t i=31; i>=0;i--)
        if(bitRead(k, i))
            return i + 1;
    return 0;
}

// simple curve order algorithm
uint32_t EllipticCurve::calcOrder(Point G)
{
    Point P = G;
    uint32_t i = 2;
    for (; !(P == InfPoint); i++){ P = addPoint(G, P); }
    return i-1;
}


bool EllipticCurve::isOnCurve(Point G)
{
    return (G.y * G.y) % p == (G.x * G.x * G.x + a * G.x + b) % p;
}
