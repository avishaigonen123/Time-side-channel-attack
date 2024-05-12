#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_MPR121.h"

#ifndef _BV
#define _BV(bit) (1 << (bit)) 
#endif
#define PASS_SIZE 4
// You can have up to 4 on one i2c bus but one is enough for testing!
Adafruit_MPR121 cap = Adafruit_MPR121();

// Keeps track of the last pins touched
// so we know when buttons are 'released'
uint16_t last_touched = 0;
uint16_t curr_touched = 0;

uint8_t password[PASS_SIZE]={3,2,1,0};

void setup() {
  Serial.begin(9600);

  while (!Serial) { // needed to keep leonardo/micro from starting too fast!
    delay(10);
  }
  
  Serial.println("Adafruit MPR121 Capacitive Touch sensor test"); 
  
  // Default address is 0x5A, if tied to 3.3V its 0x5B
  // If tied to SDA its 0x5C and if SCL then 0x5D
  if (!cap.begin(0x5A)) {
    Serial.println("MPR121 not found, check wiring?");
    while (1);
  }
  Serial.println("MPR121 found!");
}

uint8_t curr_pass[PASS_SIZE];

bool check_pass(uint8_t* pass)
{
  for (int i = 0; i < PASS_SIZE; i++)
    if( pass[i]!=password[i])
      return false;
  return true;
}

uint8_t counter = 0;
uint16_t last_time = millis(); 

void initialize_password()
{
  int count=0;

  do
  {
      // reset our state
    last_touched = curr_touched;      
    // Get the currently touched pads
    curr_touched = cap.touched();
     for (uint8_t i=0; i<12; i++) {
      // it if *is* touched and *wasnt* touched before, alert!
      if ((curr_touched & _BV(i)) && !(last_touched & _BV(i)) ) {
        password[count] = i;
        Serial.println(i);
        count++;
        break;
      }
     }
  } while (count!=PASS_SIZE);
  
  Serial.print("new password set:");
  for (int i = 0; i < PASS_SIZE; i++)
    Serial.print(password[i]); Serial.print(" ");
  
  Serial.print("\n");
  return;
}

void loop() {
  
  // Get the currently touched pads
  curr_touched = cap.touched();
  
  if(curr_touched==0x909 && last_touched!=0x909) // intialiize password 
  {
    Serial.println("initlalize password");
    initialize_password();
    counter = 0;
    last_time = millis();
  }
  else
  {
      // check success
    if(counter==PASS_SIZE)
    {
      if(check_pass(curr_pass))
      {
        Serial.print("Well done, you guess!\n");
      }
      else
      {
        Serial.print("Wrong!\n");
      }
      counter=0;
    }
    // if not touched more than 5 seconds
    if(millis()-last_time >= 5000 && counter) // and also end count
    {
        last_time = millis();
        counter=0;
        Serial.print("Time Out!\n");
        delay(10);
    }
    for (uint8_t i=0; i<12; i++) {
      // it if *is* touched and *wasnt* touched before, alert!
      if ((curr_touched & _BV(i)) && !(last_touched & _BV(i)) ) {
        curr_pass[counter] = i;
        Serial.println(i);
        counter++;
        last_time = millis();
        break;
      }
    }
    
  }
  // reset our state
  last_touched = curr_touched;
}
