#include <Wire.h>
#include <pins_energia.h>

// Define constants for your device's I2C address and I2C frequency
#define I2C_ADDRESS 0x5A
#define I2C_FREQUENCY 400000 // 400 kHz

volatile uint8_t data[4];

// Function to handle incoming I2C data
void onRec_Callback(int numOfBytes) {
    Serial.println("In recv");
    for (uint8_t i = 0; Wire.available(); i++) {
        data[i] = Wire.read();
    }
}

// Function to handle outgoing I2C data
void onReq_Callback() {
    Serial.println("In req");
    static uint16_t state = 1;
    if (data[0] == 0x5E) {
        Wire.write(0x24);
    } else if (data[0] == 0x5D) {
        Wire.write(0x0);
    } else if (data[0] == 0x00) {
        Wire.write((uint8_t)state);
        Wire.write(state >> 8);
        state = (state << 1) ? state << 1 : 1;
    }
    data[0] = 0xFF;
}

void setup() {
    // Initialize serial communication
    Serial.begin(115200);

    // Clear data array
    for (uint8_t i = 0; i < 4; i++) {
        data[i] = 0xFF;
    }

    // Initialize I2C with the specified address and set the clock frequency
    Wire.begin(I2C_ADDRESS);
    Wire.setClock(I2C_FREQUENCY);

    // Register the I2C callbacks
    Wire.onReceive(onRec_Callback);
    Wire.onRequest(onReq_Callback);
}

void loop() {
    delay(1);
}
