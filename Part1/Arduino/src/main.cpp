#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include "Adafruit_MPR121.h"

#ifndef _BV
#define _BV(bit) (1 << (bit)) 
#endif
#define PASS_SIZE 8 // here, change the pass size
#define MAX_PASS_SIZE 20
#define RED_LED 12
#define GREEN_LED 13

// SDA: A4
// SCL: A5

#define RESET_BUTTON 0x001
#define ENTER_BUTTON 0x100
#define INITIALIZE_VALUE 0x909

uint8_t password[PASS_SIZE]={1,2,3,4,5,6,7,8}; // the secret password, here, change the password

// You can have up to 4 on one i2c bus but one is enough for testing!
Adafruit_MPR121 cap = Adafruit_MPR121();

const uint8_t real_to_fake_keypad[12] = {0xFF,7,4,1,0,8,5,2,0xFF,9,6,3}; // map from keypad to how normal users handle keypad

// Keeps track of the last pins touched
// so we know when buttons are 'released'
uint16_t last_touched = 0;
uint16_t curr_touched = 0;

uint8_t curr_pass[MAX_PASS_SIZE]; // our current password

uint8_t counter = 0; // the counter of where are we now
uint16_t last_time = millis(); // count the last time, in order to manage Time out


// setup func
void setup() {
  // Initialize serial communication at 115200 baud
  Serial.begin(115200);

  // Wait for serial port to connect. Needed for Leonardo/Micro only
  while (!Serial) { 
    delay(10);
  }
  
  // Set up LED pins as output and initialize them to low
  pinMode(GREEN_LED, OUTPUT); // green LED
  pinMode(RED_LED, OUTPUT); // red LED
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);

  // Print a message to the serial monitor
  Serial.println("Adafruit MPR121 Capacitive Touch sensor test"); 
  
  // Default address is 0x5A, if tied to 3.3V its 0x5B
  // If tied to SDA its 0x5C and if SCL then 0x5D
  // Begin the MPR121 sensor with the specified address
  if (!cap.begin(0x5A)) {
    // Print an error message if the sensor is not found
    Serial.println("MPR121 not found, check wiring?");
    // Stop the program if the sensor is not found
    while (1);
  }
  // Print a success message if the sensor is found
  Serial.println("MPR121 found!");
}

// the vulnerable check password func
bool check_pass()
{
    for (int i = 0; i < PASS_SIZE; i++){ // this is the vulnerable part of the code
        if(curr_pass[i]!=password[i]) 
            return false;
    }
    return counter <= PASS_SIZE;
}

// some manage functions, to manage possible cases
void manage_wrong_pass()
{
	digitalWrite(RED_LED, HIGH);
	Serial.print("Wrong!\n");
}

void manage_good_pass()
{
    digitalWrite(GREEN_LED, HIGH);
    Serial.print("Well done!\n");
}

void manage_overflow()
{
  	digitalWrite(RED_LED, HIGH);
  	Serial.print("Too much characters!\n");     
}

void manage_reset() 
{
	Serial.println("reset button");
  // blink red LED
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

void manage_initialize_password() 
{
  // function that let user to change password using touching some special characters
    // blink red LED
  	digitalWrite(RED_LED, LOW);
    delay(200);
    digitalWrite(RED_LED, HIGH);
    delay(200);
    digitalWrite(RED_LED, LOW);
    delay(200);
    digitalWrite(RED_LED, HIGH);
    delay(200);
    digitalWrite(RED_LED, LOW);
  int count=0;
  do
  {
      // reset our state
    last_touched = curr_touched;      
    // Get the currently touched pads
    curr_touched = cap.touched();
     for (uint8_t i=0; i<12; i++) {
      // it if is touched and wasnt touched before, alert!
      if ((curr_touched & _BV(i)) && !(last_touched & _BV(i)) ) {
        password[count] = real_to_fake_keypad[i];
        Serial.println(real_to_fake_keypad[i]);
        count++;
        break;
      }
     }
  } while (count!=PASS_SIZE);
  
    Serial.print("new password set:");
    for (int i = 0; i < PASS_SIZE; i++){
        Serial.print(password[i]); Serial.print(" ");
    }
    Serial.println();

    digitalWrite(GREEN_LED, HIGH);
    delay(200);
    digitalWrite(GREEN_LED, LOW);

}

// the main loop of the program
void loop() {
  
 	// Get the currently touched pads
  	curr_touched = cap.touched();
  
  	if(curr_touched != last_touched) // if the user touched the keypad
		switch (curr_touched) // manage different cases
		{
		case INITIALIZE_VALUE: // initialize password
			Serial.println("initialize password");
			manage_initialize_password();
			last_time = millis();	
			
			counter=0;	
			break;
		
		case ENTER_BUTTON: // enter button
			Serial.println("e");
			if(check_pass() && counter!=0) 
				manage_good_pass();
			else
				manage_wrong_pass();
			counter=0;
			last_touched=curr_touched;		
			break;
		
		case RESET_BUTTON: // reset button
		    manage_reset();
			counter=0;
			last_touched=curr_touched;		
			break;
		}

  // if not touched more than 5 seconds, need to do Time Out
	if((uint16_t)(millis())-last_time >= 5000 && counter) // and also end count
  {
      last_time = millis();
      counter=0;
      Serial.print("Time Out!\n");
      delay(10);
  }

  // get the current digit touched by the user
  for (uint8_t i=0; i<12; i++) {
    // it if is touched and wasnt touched before, alert!
    if ((curr_touched & _BV(i)) && !(last_touched & _BV(i)) ) { // check if something was touched, and is different from last touched value
      curr_pass[counter] = real_to_fake_keypad[i];
      Serial.println(real_to_fake_keypad[i]); // print the current value touched by the user
      counter++; // increment counter of where are we in current password
      last_time = millis(); // update last time
      digitalWrite(RED_LED, LOW);
      digitalWrite(GREEN_LED, LOW);
      break; 
    }
  }

  // we reached max limit of the password
  if(counter==MAX_PASS_SIZE)
	{
		manage_overflow();    
		counter = 0;
  }
  
  // set last touched to curr_touched
  last_touched = curr_touched;
}