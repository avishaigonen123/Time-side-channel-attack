#include "PointStream.h"
/**
 * a privet func to add a crc to the buff
 * must be alocated in advence for DATA_SIZE
 */
void PointStream::addCRC(uint8_t* buff){
    uint8_t crc = 0;
    for (size_t i = 0; i < sizeof(Point); i++)
        crc ^= buff[i];
    // Serial.print("T:");
    // Serial.println(crc);
    buff[sizeof(Point)] = crc;
}
/**
 * a privet func to check if the crc is valid
 * must be alocated in advence for DATA_SIZE
 */
bool PointStream::checkCRC(uint8_t *buff){
    uint8_t crc = 0;
    for (size_t i = 0; i < sizeof(Point); i++)
        crc ^= buff[i];
    // Serial.print("R:");
    // Serial.println(crc);
    return buff[sizeof(Point)] == crc;
}

void printBuff(const char* TAG, uint8_t *buff, size_t size){
    Serial.print(TAG);
    Serial.print(": ");
    if (size == 0)
        return; 
    Serial.print(buff[0], HEX);
    for (size_t i = 1; i < size; i++){
        Serial.print(" ");
        Serial.print(buff[i], HEX);
    }
    Serial.println();
}

void PointStream::send(Point* source){
    uint8_t buff[PACKET_SIZE];
    memset(buff, 0, PACKET_SIZE);
    buff[0] = START_SEQUENCE;
    memcpy(&(buff[1]), source, sizeof(Point));
    addCRC(&(buff[1]));
    buff[PACKET_SIZE-1] = STOP_SEQUENCE;
    stream->write(buff, PACKET_SIZE);
}

bool PointStream::Recive(Point* dest)
{
    uint8_t buff[DATA_SIZE];
    memset(buff, 0, DATA_SIZE);
    //printBuff("R1",buff, DATA_SIZE);

    while(stream->available() && stream->read() != START_SEQUENCE);
    if (stream->available() == 0)
        return false;
    //printBuff("R2",buff, DATA_SIZE);

    stream->readBytes(buff, DATA_SIZE);

    //printBuff("R3",buff, DATA_SIZE);

    if(!checkCRC(buff))
        return false;

    memcpy(dest, buff, sizeof(Point));

    return true;
}

bool PointStream::sendAndRecive(Point* source, Point* dest)
{
    send(source);
    while(!stream->available());
    return Recive(dest);
}

