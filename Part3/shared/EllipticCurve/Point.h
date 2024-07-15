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

    Point(){
        x = -1;
        y = -1;
    }

    Point(uint32_t _x, uint32_t _y){
        x = _x;
        y = _y;
    }

    void print(Stream &out = Serial){
        out.print(x);
        out.print(",");
        out.print(y);
		out.println();
    }

} Point;

#endif // POINT