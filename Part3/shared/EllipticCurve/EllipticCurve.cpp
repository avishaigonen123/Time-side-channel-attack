#include "EllipticCurve.h"
#include <Arduino.h>

// Function that performs point adding
Point EllipticCurve::addPoint(const Point& point1, const Point& point2)
{
    if (point1.x == point2.x) // next point will be infinity point
        return InfPoint;    
    if (point1 == InfPoint) // return the second point
        return point2;
    if (point2 == InfPoint) // return the second point
        return point1;
    if (point1.x == point2.x && point1.y == point2.y) // the same point
        return doublingPoint(point1);  
    Point R;
    int32_t numerator = point1.y - point2.y;
    int32_t denominator = point1.x - point2.x;
    int32_t s = special_module(numerator, p) * modularInverse(denominator, p);

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
    int32_t s = module(numerator, p) * modularInverse(denominator, p);

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
    }
    return R0;
}

// Function to calculate the point on the elliptic curve given the private key
Point EllipticCurve::EllipticCurveCalcPoint(Point P, uint32_t PrivKey){
    return scalarMultiplication(P, PrivKey); // use the scalar multiplication algorithm
}

// Function to calculate gcd(a, b) using Euclidean algorithm
int EllipticCurve::gcdExtended(int a, int b, int *x, int *y) {
    // Base case
    if (a == 0) {
        *x = 0, *y = 1;
        return b;
    }

    int x1, y1; // To store results of recursive call
    int gcd = gcdExtended(b % a, a, &x1, &y1);

    // Update x and y using results of recursive call
    *x = y1 - (b / a) * x1;
    *y = x1;

    return gcd;
}

// Function to find modular inverse of a under regular_modulo p
int EllipticCurve::modularInverse(int a, int p) {
    int x, y;
    int gcd = gcdExtended(a, p, &x, &y);

    if (gcd != 1) {
        // Modular inverse doesn't exist
        return -1;
    } else {
        // Handling negative x to ensure it's positive
        return (x % p + p) % p;
    }
}

// this function calculates the length in bits of the key
byte EllipticCurve::key_length(uint32_t k)
{
    for (uint8_t i=31; i>=0;i--)
        if(bitRead(k, i))
            return i + 1;
    return 0;
}

// simple modulo
uint32_t EllipticCurve::module(int32_t a, int32_t b) {
    int r = a % b;
    if (r < 0)
        r += b;
    return r;
}

// special modulo
uint32_t EllipticCurve::special_module(int32_t a, int32_t b) {
    int r = a % b;
    if (r < 0){
        delay(5);
        r += b;
    }
    return r;
}
