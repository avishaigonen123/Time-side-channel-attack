#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include "Adafruit_MPR121.h"

#ifndef _BV
#define _BV(bit) (1 << (bit)) 
#endif
#define PASS_SIZE 4
#define MAX_PASS_SIZE 20
#define RED_LED 12
#define GREEN_LED 13

#define RESET_BUTTON 0x001
#define ENTER_BUTTON 0x100
#define INITIALIZE_VALUE 0x909

const uint8_t real_to_fake_keypad[12] = {0xFF,7,4,1,0,8,5,2,0xFF,9,6,3};
// You can have up to 4 on one i2c bus but one is enough for testing!
Adafruit_MPR121 cap = Adafruit_MPR121();

// Keeps track of the last pins touched
// so we know when buttons are 'released'
uint16_t last_touched = 0;
uint16_t curr_touched = 0;

uint8_t password[PASS_SIZE]={2,5,8,0};

void setup() {
  /* Serial.begin(9600);

  while (!Serial) { // needed to keep leonardo/micro from starting too fast!
    delay(10);
  }
  */
  pinMode(GREEN_LED, OUTPUT); // green LED
  pinMode(12, OUTPUT); // red LED

  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);

  // Serial.println("Adafruit MPR121 Capacitive Touch sensor test"); 
  
  // Default address is 0x5A, if tied to 3.3V its 0x5B
  // If tied to SDA its 0x5C and if SCL then 0x5D
  if (!cap.begin(0x5A)) {
    // Serial.println("MPR121 not found, check wiring?");
    while (1);
  }
  // Serial.println("MPR121 found!");
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
    delayMicroseconds(100); 
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

void manage_initialize_password() 
{
    
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
        // Serial.println(real_to_fake_keypad[i]);
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

void loop() {
  
 	// Get the currently touched pads
  	curr_touched = cap.touched();
  
  	if(curr_touched != last_touched) 
		switch (curr_touched)
		{
		case INITIALIZE_VALUE: // initialize password
			Serial.println("initialize password");
			manage_initialize_password();
			last_time = millis();	
			
			counter=0;	
			break;
		
		case ENTER_BUTTON: // enter button
			// Serial.println("enter button");
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


  
// if not touched more than 5 seconds
	if((uint16_t)(millis())-last_time >= 5000 && counter) // and also end count
    {
        last_time = millis();
        counter=0;
        Serial.print("Time Out!\n");
        delay(10);
    }
    for (uint8_t i=0; i<12; i++) {
      // it if is touched and wasnt touched before, alert!
      if ((curr_touched & _BV(i)) && !(last_touched & _BV(i)) ) {
        curr_pass[counter] = real_to_fake_keypad[i];
        Serial.println(real_to_fake_keypad[i]);
        counter++;
        last_time = millis();
        digitalWrite(RED_LED, LOW);
        digitalWrite(GREEN_LED, LOW);
        break;
      }
    }

    if(counter==MAX_PASS_SIZE)
	{
		manage_overflow();    
		counter = 0;
  	}
  
  // reset our state
  last_touched = curr_touched;
}