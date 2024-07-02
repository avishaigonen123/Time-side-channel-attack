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

#define PASS_SIZE 4
#define MAX_PASS_SIZE 20
#define RED_LED 12
#define GREEN_LED 13
#define GO_pin 10

#define RESET_BUTTON '*'
#define ENTER_BUTTON '#'
// #define INITIALIZE_VALUE ''

uint8_t password[PASS_SIZE]={8,5,8,0};

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
  pinMode(GO_pin, OUTPUT); // red LED

  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);

  customKeypad.setDebounceTime(1);
}

uint8_t curr_pass[MAX_PASS_SIZE];
uint8_t counter = 0;
uint16_t last_time = millis(); 

bool check_pass()
{
	for (int i = 0; i < PASS_SIZE; i++)
	{
		if( curr_pass[i]!=password[i])
		return false;
		delay(100);
	}
	return counter <= PASS_SIZE;  // because he might give the password+some other stuff
}



void manage_wrong_pass()
{
	digitalWrite(RED_LED, HIGH);
	// Serial.print("Wrong!\n");
}

void manage_good_pass()
{
	digitalWrite(GREEN_LED, HIGH);
	Serial.print("Well done, you guess!\n");
}

void manage_overflow()
{
  	digitalWrite(RED_LED, HIGH);
  	Serial.print("Too much characters!\n");     
}

void manage_reset()
{
	Serial.println("reset button");
	digitalWrite(GREEN_LED, LOW);
  	digitalWrite(RED_LED, LOW);
    delay(200);
    digitalWrite(RED_LED, HIGH);
    delay(200);
    digitalWrite(RED_LED, LOW);
    delay(200);
    digitalWrite(RED_LED, HIGH);
    delay(200);
    digitalWrite(RED_LED, LOW);
}


void loop() {
  
	// Get key value if pressed
	char customKey = customKeypad.getKey();
	if(customKey)
	{
		Serial.println(customKey);
		last_time = millis();	

		switch (customKey)
			{
			case ENTER_BUTTON: // enter button
				// Serial.println("enter button");
				if(check_pass() && counter!=0)
					manage_good_pass();
				else{
					manage_wrong_pass();
				}
				counter=0;
				break;
			
			case RESET_BUTTON: // reset button
				manage_reset();
				counter=0;
				break;
		default: 
				curr_pass[counter] = int(customKey-'0');
				counter++;
				last_time = millis();
				digitalWrite(RED_LED, LOW);
				digitalWrite(GREEN_LED, LOW);
			break;
		}
	}
	// if not touched more than 5 seconds
	if((uint16_t)(millis())-last_time >= 5000 && counter) // and also end count
	{
		last_time = millis();
		counter=0;
		Serial.print("Time Out!\n");
		delay(10);
	}

	if(counter==MAX_PASS_SIZE)
	{
		manage_overflow();    
		counter = 0;
	}
}