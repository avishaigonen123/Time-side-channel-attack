#include <Arduino.h>
#include <wire.h>

TwoWire arduino = TwoWire(0x5A);

void setup() {
	Serial.begin(115200);
	Wire.begin();
}

void loop() {
	Serial.printf("gonen is metonym beyoter");
}