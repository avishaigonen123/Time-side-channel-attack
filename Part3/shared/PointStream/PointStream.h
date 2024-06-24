#ifndef POINT_STREAM_H
#define POINT_STREAM_H
#include <Arduino.h>
#include <Stream.h>
#include "../EllipticCurve/Point.h"

#define DATA_SIZE (sizeof(Point) + sizeof(byte))
#define PACKET_SIZE (DATA_SIZE + 2)
#define START_SEQUENCE 'S'
#define STOP_SEQUENCE 'F'


class PointStream{
    Stream* stream;
    void addCRC(uint8_t* buff);
    bool checkCRC(uint8_t* buff);
public:
    PointStream(Stream* _stream): stream(_stream) {}
    bool sendAndRecive(Point* source, Point* dest);
    void send(Point* source);
    bool Recive(Point* dest);
};

#endif // POINT_STREAM_H