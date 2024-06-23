#include <Arduino.h>
#include <SoftwareSerial.h>

// start the software serial, for communication with the ESP32
SoftwareSerial SerialESP32(6,7);

int modularInverse(int, int);
uint32_t modulo(int32_t, int32_t);
byte key_length(uint32_t);

typedef struct Point{
	uint32_t x, y;

    bool operator==(const Point& p1) const{ // implementation of point comparison
        return x == p1.x && y == p1.y;
    }

    void print(){
        SerialESP32.print(this->x); SerialESP32.print(" "); SerialESP32.println(this->y);
    }

}Point;


class EllipticCurve{
    // default values 
    uint32_t a=4;  //coefficient for elliptic curve
    uint32_t b=20; //coefficient for elliptic curve
    uint32_t p=29; //prime number to provide finite field

    const Point InfPoint = {UINT32_MAX, UINT32_MAX};

public:
    EllipticCurve(){}
    EllipticCurve(uint32_t _a,uint32_t _b,uint32_t _p):a(_a),b(_b),p(_p){}


    // implementation of Adding
    Point addPoint(const Point& point1, const Point& point2)
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
        int32_t s = modulo(numerator, p) * modularInverse(denominator, p);

        R.x = s * s - (point1.x + point2.x);
        R.x = modulo(R.x, p);
        R.y = s * (point1.x - R.x) - point1.y;
        R.y = modulo(R.y, p);
        return R;
    }
    
    // implementation of doubling point, based on 
    Point doublingPoint(const Point& point)
    {
        if (point == InfPoint) // InfPoint+InfPoint = InfPoint
            return InfPoint;
        if (point.y == 0) // InfPoint because this is a vertical line
            return InfPoint;
        Point R;
        int32_t numerator = 3 * point.x * point.x + a;
        int32_t denominator = 2 * point.y;
        int32_t s = modulo(numerator, p) * modularInverse(denominator, p);

        R.x = s * s - 2 * point.x;
        R.x = modulo(R.x, p);
        R.y = s * (point.x - R.x) - point.y;
        R.y = modulo(R.y, p);
        return R;
    }
        
    // the function is the vulnerable implementation of the algorithm
    Point scalarMultiplication(Point P, uint32_t k)
    {
        Point R0 = P;
        byte l = key_length(k);
        for(int8_t j = l-2; j >= 0; j--)
        {
            R0 = doublingPoint(R0);
            if(bitRead(k, j))
                R0 = addPoint(R0, P);
        }
        return R0;
    }

    // for modularization of the program, that we'll be able to use different implementations
    Point EllipticCurveCalcPoint(Point P, uint32_t PrivKey)
    {
        return scalarMultiplication(P, PrivKey);
    }

};
// fix modulo that it will handle properly negative numbers
uint32_t modulo(int32_t a, int32_t b) {
    int r = a % b;
    return r >= 0 ? r : r + b;
}


// Function to calculate gcd(a, b) using Euclidean algorithm
int gcdExtended(int a, int b, int *x, int *y) {
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

// Function to find modular inverse of a under modulo p
int modularInverse(int a, int p) {
    int x, y;
    int gcd = gcdExtended(a, p, &x, &y);

    if (gcd != 1) {
        // Modular inverse doesn't exist
        return -1;
    } else {
        // Handling negative x to ensure it's positive
        int inverse = (x % p + p) % p;
        return inverse;
    }
}


// this function calculates the length in bits of the key
byte key_length(uint32_t k)
{
    for (uint8_t i=31; i>=0;i--)
        if(bitRead(k, i))
            return i + 1;
    return 0;
}


uint32_t privKey = 49;
uint32_t a = 5;
uint32_t b = 2;
uint32_t p = 97;
EllipticCurve curve(a,b,p);

Point point;
char buffer[sizeof(Point)];

void setup(){
    SerialESP32.begin(115200);
    Serial.begin(115200);
}

void loop(){
    if(SerialESP32.available())
    {
        // get point from client
        SerialESP32.readBytes(buffer, sizeof(Point));   
        memcpy(&point, buffer, sizeof(Point));
        
        // send point after algorithm
        point = curve.EllipticCurveCalcPoint(point, privKey);
        SerialESP32.write((uint8_t*)&point, sizeof(Point));
    }
}