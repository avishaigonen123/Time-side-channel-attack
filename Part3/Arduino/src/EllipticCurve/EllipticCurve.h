#ifndef ELLIPTIC_CURVE_H 
#define ELLIPTIC_CURVE_H
#include <stdint.h>
#include "Point.h"

class EllipticCurve{
    // default values 
    uint32_t a;  //coefficient for elliptic curve
    uint32_t b; //coefficient for elliptic curve
    uint32_t p; //prime number to provide finite field
    int gcdExtended(int a, int b, int *x, int *y);
    int modularInverse(int, int);
    uint32_t modulo(int32_t, int32_t);
    uint8_t key_length(uint32_t);
public:
    const Point InfPoint = {UINT32_MAX, UINT32_MAX};
    EllipticCurve(){}
    EllipticCurve(uint32_t _a,uint32_t _b,uint32_t _p):a(_a),b(_b),p(_p){}

    // implementation of Adding
    Point addPoint(const Point& point1, const Point& point2);
    
    // implementation of doubling point, based on 
    Point doublingPoint(const Point& point);
        
    // the function is the vulnerable implementation of the algorithm
    Point scalarMultiplication(Point P, uint32_t k);

    // for modularization of the program, that we'll be able to use different implementations
    Point EllipticCurveCalcPoint(Point P, uint32_t PrivKey);
};

#endif // ELLIPTIC_CURVE_H