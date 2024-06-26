#ifndef POINT_H
#define POINT_H

#include <stdint.h>
#include <Stream.h>
#include <Arduino.h>

typedef struct __attribute__ ((packed)) Point{
	uint32_t x, y;

    bool operator==(const Point& p1) const{ // implementation of point comparison
        return x == p1.x && y == p1.y;
    }

    void print(Stream* out = &Serial){
        out->print(this->x); out->print(" "); out->println(this->y);
    }

}Point;

#endif // POINT