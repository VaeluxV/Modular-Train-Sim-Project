#include <Arduino.h>
#include <Wire.h>

// Define the I2C device address
#define DEVICE_ADDRESS 0x08

// Global variables
int myIntVariable;
float myFloatVariable;
char myCharVariable;

int throttle;
int rawThrottle;
int combinedThrottleBrake;
int rawCombinedThrottleBrake;
int brake;
int rawBrake;
int reverser;
int rawReverser;

// Function prototypes
void receiveEvent(int bytes);
bool compareType(const char* varName, const char* typeName);
void printError(const char* message);

void setup() {
  Wire.begin(DEVICE_ADDRESS);  // Initialize I2C communication with the specified address
  Wire.onReceive(receiveEvent);  // Register event handler for receiving data
  Serial.begin(9600);  // Initialize serial communication for debugging
}

void loop() {
  // Main loop does nothing, everything is handled in receiveEvent
  delay(100);
}

/*
  Event handler for receiving I2C data
  @param bytes The number of bytes received
*/
void receiveEvent(int bytes) {
  if (bytes <= 0) {
    return;
  }

  char varName[32];
  int varIndex = 0;

  // Read variable name
  while (Wire.available() > 1 && varIndex < sizeof(varName) - 1) {
    char c = Wire.read();
    if (c == ':') {
      break;
    }
    varName[varIndex++] = c;
  }
  varName[varIndex] = '\0';  // Null-terminate the string

  // Read the variable value
  if (strcmp(varName, "myIntVariable") == 0) {
    int value;
    Wire.readBytes(reinterpret_cast<char*>(&value), sizeof(value));
    if (compareType(varName, "int")) {
      myIntVariable = value;
    } else {
      printError("Type mismatch for myIntVariable");
    }
  } else if (strcmp(varName, "myFloatVariable") == 0) {
    float value;
    Wire.readBytes(reinterpret_cast<char*>(&value), sizeof(value));
    if (compareType(varName, "float")) {
      myFloatVariable = value;
    } else {
      printError("Type mismatch for myFloatVariable");
    }
  } else if (strcmp(varName, "myCharVariable") == 0) {
    char value;
    Wire.readBytes(reinterpret_cast<char*>(&value), sizeof(value));
    if (compareType(varName, "char")) {
      myCharVariable = value;
    } else {
      printError("Type mismatch for myCharVariable");
    }
  } else if (strcmp(varName, "rawThrottle") == 0) {
    int value;
    Wire.readBytes(reinterpret_cast<char*>(&value), sizeof(value));
    if (compareType(varName, "int")) {
      rawThrottle = value;
    } else {
      printError("Type mismatch for rawThrottle");
    }
  } else if (strcmp(varName, "rawCombinedThrottleBrake") == 0) {
    int value;
    Wire.readBytes(reinterpret_cast<char*>(&value), sizeof(value));
    if (compareType(varName, "int")) {
      rawCombinedThrottleBrake = value;
    } else {
      printError("Type mismatch for rawCombinedThrottleBrake");
    }
  } else if (strcmp(varName, "rawBrake") == 0) {
    int value;
    Wire.readBytes(reinterpret_cast<char*>(&value), sizeof(value));
    if (compareType(varName, "int")) {
      rawBrake = value;
    } else {
      printError("Type mismatch for rawBrake");
    }
  } else if (strcmp(varName, "rawReverser") == 0) {
    int value;
    Wire.readBytes(reinterpret_cast<char*>(&value), sizeof(value));
    if (compareType(varName, "int")) {
      rawReverser = value;
    } else {
      printError("Type mismatch for rawReverser");
    }
  } else {
    printError("Unknown variable name");
  }

  Serial.println(myIntVariable);
  Serial.println(myFloatVariable);
  Serial.println(myCharVariable);
}

/*
  Function to compare the type of a variable
  @param varName The name of the variable
  @param typeName The expected type of the variable as a string
  @return True if the types match, false otherwise
*/
bool compareType(const char* varName, const char* typeName) {
  if (strcmp(varName, "myIntVariable") == 0 && strcmp(typeName, "int") == 0) {
    return true;
  } else if (strcmp(varName, "myFloatVariable") == 0 && strcmp(typeName, "float") == 0) {
    return true;
  } else if (strcmp(varName, "myCharVariable") == 0 && strcmp(typeName, "char") == 0) {
    return true;
  } else if (strcmp(varName, "rawThrottle") == 0 && strcmp(typeName, "int") == 0) {
    return true;
  } else if (strcmp(varName, "rawCombinedThrottleBrake") == 0 && strcmp(typeName, "int") == 0) {
    return true;
  } else if (strcmp(varName, "rawBrake") == 0 && strcmp(typeName, "int") == 0) {
    return true;
  } else if (strcmp(varName, "rawReverser") == 0 && strcmp(typeName, "int") == 0) {
    return true;
  }
  return false;
}

/*
  Function to print an error message to the serial console
  @param message The error message
*/
void printError(const char* message) {
  Serial.println(message);
}
