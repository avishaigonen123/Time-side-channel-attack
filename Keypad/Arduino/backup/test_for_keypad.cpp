/*
  Matrix Keypad Demo
  keypad-demo.ino
  Demonstrates use of 4x4 matrix membrane keypad with Arduino
  Results on serial monitor

  DroneBot Workshop 2020
  https://dronebotworkshop.com
*/

// Include the Keypad library
#include <Keypad.h>
#include <Arduino.h>
#include <Wire.h>

#define PASS_SIZE 4
#define MAX_PASS_SIZE 20
#define RED_LED 12
#define GREEN_LED 13
#define START_PIN 10

#define RESET_BUTTON '*'
#define ENTER_BUTTON '#'

// Constants for row and column sizes
const byte ROWS = 4;
const byte COLS = 3;

// Array to represent keys on keypad
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

// Connections to Arduino
byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3};

// Create keypad object
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

void setup() {
  // Setup serial monitor
  Serial.begin(9600);

  pinMode(GREEN_LED, OUTPUT); // green LED
  pinMode(RED_LED, OUTPUT); // red LED
  pinMode(START_PIN, INPUT_PULLUP); // red LED

  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);

  customKeypad.setDebounceTime(0);
}

byte sum = 0;
uint32_t globalCount = 0;
uint32_t perfectCount = 0;

void loop() {
	if(globalCount<100){
		char customKey = customKeypad.getKey();
		if(customKey){
			if(customKey <= '9' && customKey >= '0'){
				sum = (sum==(customKey-'0'))?sum+1:sum;
			}
			else if(customKey == '*'){
				globalCount++;
				if(sum == 10){
					perfectCount++;
				}
				else{
					Serial.print(" miss: ");
					Serial.print(sum);
					Serial.print(" | ");
				}
				Serial.print(perfectCount);
				Serial.print("/");
				Serial.print(globalCount);
				Serial.println();
				sum = 0;
			}
		}
	}
}