#include <Arduino.h>
#include <pins_arduino.h>
#include <Wire.h>

#define PASS_SIZE 4

#define PIN_FAIL 18
#define PIN_SUCCESS 19

volatile uint8_t data[4];
uint8_t pass[PASS_SIZE];
uint8_t to_send[PASS_SIZE];

uint8_t available_numbers[12] = {0,1,2,3,4,5,6,7,8,9,10,11}; 
bool in_handle = false;

uint8_t left_to_send = 4;
void send_handle_func()
{
    if(left_to_send == 0)
    {
        for (int i = 0; i < 12; i++)
        {
            if(to_send[0]==12)
            {
                to_send[0] = 0;
                if(to_send[1]==12)
                {
                    to_send[1] = 0;
                    if(to_send[2]==12)
                    {
                        to_send[2] = 0;
                        if(to_send[3]==12) 
                        {   
                            to_send[3] = 0;
                        }   
                        else
                            to_send[3]++;
                    }
                    else
                        to_send[2]++;
                }
                else
                    to_send[1]++;

            }
            else
                to_send[0]++;
                Serial.print("here");        }
        left_to_send=4;
    }
    Serial.println(to_send[4-left_to_send]);
    Wire.write((uint8_t)to_send[4-left_to_send]);Wire.write(to_send[4-left_to_send] >> 8);
}

void onRec_Callback(int numOfBytes){
  
    for(uint8_t i = 0 ; Wire.available() ; i++)
    {
        data[i] = Wire.read();
        //Serial.print(, HEX);
        //Serial.print(" ");
    }
}
void onReq_Callback(){
    static uint16_t pass[PASS_SIZE];
    if(data[0] == 0x5E){
        Wire.write(0x24);
        //Serial.println("sent 0x24");
    }
    else if(data[0] == 0x5D){ // not touching anything on keypad
        Wire.write(0x0);
        //Serial.println("sent 0x0");
    }
    else if(data[0] == 0x00){ // read from reg0, after touch
        // Wire.write((uint8_t)state);Wire.write(state >> 8);
        // state = (state << 1)?state << 1:1;
        if(!in_handle)
            in_handle = true;
            send_handle_func();
        //Serial.printf("data[0]: %u state: %u state\n",data[0], state, state & (uint16_t)0xFF00);
    }
    //Serial.println("\n++++++++++++++++++++++++++++++++++++");
    data[0] = 0xFF;
}

void setup() {
    srand(time(NULL));
    Serial.begin(115200);
    for (uint8_t i = 0; i < 4; i++)
    {
        data[i] = 0xFF;
    }
    
    pinMode(PIN_FAIL, INPUT);
    pinMode(PIN_SUCCESS, INPUT);

    Wire.begin(0x5A, SDA, SCL, 400000);
    Wire.onReceive(onRec_Callback);
    Wire.onRequest(onReq_Callback);
}

void loop() {
    delay(1);
}