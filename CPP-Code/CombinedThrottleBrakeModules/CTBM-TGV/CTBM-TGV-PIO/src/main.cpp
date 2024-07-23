#include <Arduino.h>
#include <Wire.h>

#define DEVICE_ADDRESS 0x08  // Define the I2C device address

#define SEND_RATE 120 // Define the amount of times per second to send data

unsigned long interval = 1000 / SEND_RATE; // Interval in milliseconds
unsigned long previousMillis = 0;

int throttleSensorVal = 0;
int reverserSensorVal = 0;

/*
  Function to send a variable name and value over I2C
  @param varName The name of the variable to send
  @param varValue The value of the variable to send
  @param address The I2C device address (default is defined by DEVICE_ADDRESS)
*/

template<typename T>
void sendVariable(const char* varName, T varValue, uint8_t address = DEVICE_ADDRESS) {
  Wire.beginTransmission(address);  // Begin transmission to the I2C device
  Wire.write(varName);  // Send the variable name
  Wire.write(':');  // Send a separator
  Wire.write(reinterpret_cast<const uint8_t*>(&varValue), sizeof(T));  // Send the variable value
  Wire.endTransmission();  // End transmission
}

void setup() {
  Wire.begin();  // Initialize I2C communication
  Serial.begin(9600);  // Initialize serial communication for debugging
}

void loop() {
  unsigned long currentMillis = millis();

  // Check if the interval time has passed
  if (currentMillis - previousMillis >= interval) {
    // Save the last time you sent data
    previousMillis = currentMillis;

    // Read sensor values
    //WIP

    // Send sensor values
    sendVariable("rawCombinedThrottleBrake", throttleSensorVal);  // Send throttle
    sendVariable("rawReverser", reverserSensorVal);  // Send reverser

    //Test code only
    throttleSensorVal = throttleSensorVal++;
    reverserSensorVal = reverserSensorVal++;

    Serial.println("Data sent at: " + String(currentMillis) + "ms");
  }
}
