#include <Arduino.h>
#include <Wire.h>

#define DEVICE_ADDRESS 0x08  // Define the I2C device address

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
  Wire.write(':');  // Send a separator (optional)
  Wire.write(reinterpret_cast<const uint8_t*>(&varValue), sizeof(T));  // Send the variable value
  Wire.endTransmission();  // End transmission
}

void setup() {
  Wire.begin();  // Initialize I2C communication
  Serial.begin(9600);  // Initialize serial communication for debugging
}

void loop() {
  int myIntVariable = 42;  // Example int variable
  float myFloatVariable = 3.14;  // Example float variable
  char myCharVariable = 'A';  // Example char variable
  sendVariable("myIntVariable", myIntVariable);  // Send int variable
  delay(1000);  // Wait for a second
  sendVariable("myFloatVariable", myFloatVariable);  // Send float variable
  delay(1000);  // Wait for a second
  sendVariable("myCharVariable", myCharVariable);  // Send char variable
  delay(1000);  // Wait for a second
}



