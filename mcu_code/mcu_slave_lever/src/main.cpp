#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>
#include <Update.h>
#include <ArduinoJson.h>
#include <painlessMesh.h>

#define FORMAT_SPIFFS_IF_FAILED false

unsigned long previousMillis = 0;   // For tracking time between updates
unsigned long updateInterval = 100; // Update interval in ms (0.1 second)

int rawSensor0Data;     // For storing the raw sensor data
int prevRawSensor0Data; // For storing the previous sensor data

int processedSensor0Data;     // For storing the processed sensor data (example: smoothed out data)
int prevProcessedSensor0Data; // For storing previous state of processed sensor data. Used for the program to know if it needs to send new sensor data or not

int sensor0Percentage;
int prevSensor0Percentage;

int sensor0Pin; // The physical pin for sensor 0
int sensor0Smoothing; // How much smoothin will be applied to sensor 0
int sensor0MinValue; // The min reading of sensor 0 when lever is all the way at the minimum point
int sensor0MaxValue; // The max reading of sensor 0 when lever is all the way at the maximum point

int sensorUpdateRate; // How many readings per second you want the sensors to take. Default: 60, you can change this in the config.

/* ---  Mesh data  --- */
String meshName;
String meshPassword;
String deviceType;
String deviceID;

int meshPort;
/* --- --- --- --- --- */

painlessMesh mesh;

// Define custom functions to prevent issues with the order of these
void loadConfig();

void setupMesh();
void receivedCallback(uint32_t from, String &msg);
void newConnectionCallback(uint32_t nodeId);
void sendMessageToMesh(String msg);
float smoothSensorData(int sensorDataIn, int decimals, int readingListLength);
void sensorReading();

// Loads and parses the configuration (config.json) file
void loadConfig()
{
  // Open the configuration file
  File configFile = SPIFFS.open("/config.json", "r");
  String fileContent = "";
  if (!configFile)
  {
    Serial.println("Failed to open config file for reading");
    return;
  }

  while (configFile.available())
  {
    fileContent += (char)configFile.read();
  }
  configFile.close();

  Serial.println(fileContent);

  // Create a JSON document
  JsonDocument doc; // Increase size if necessary

  DeserializationError error = deserializeJson(doc, fileContent);

  if (error)
  {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  meshName = doc["meshName"].as<String>();              // "traincabmesh" by default, please change this in the config.json file
  meshPassword = doc["meshPassword"].as<String>();      // "traincabmesh_ABCDEFG123456" by default, please change this in the config.json file
  meshPort = doc["meshPort"].as<int>();                 // Port for the mesh network, has to be the same on all devices.
  deviceType = doc["deviceType"].as<String>();          // "slave" or "master"
  deviceID = doc["deviceID"].as<String>();              // Unique device ID
  sensorUpdateRate = doc["sensorUpdateRate"].as<int>(); // Get the sensor update rate (in times per second), 120 by default
  sensor0Pin = doc["sensor0Pin"].as<int>();             // Sensor 0 physical pin connection
  sensor0Smoothing = doc["sensor0Smoothing"].as<int>(); // Sensor 0 smoothing amount
  sensor0MinValue = doc["sensor0MinValue"].as<int>();   // Sensor 0 min value
  sensor0MaxValue = doc["sensor0MaxValue"].as<int>();   // Sensor 0 max value
}

// Initialize mesh network
void setupMesh()
{
  mesh.setDebugMsgTypes(ERROR | STARTUP | CONNECTION);         // Enable debugging
  mesh.init(meshName.c_str(), meshPassword.c_str(), meshPort); // Initialize mesh with name, password, and port 5555
  mesh.onReceive(&receivedCallback);                           // Set callback for receiving messages
  mesh.onNewConnection(&newConnectionCallback);                // Set callback for new connections
}

void receivedCallback(uint32_t from, String &msg)
{
  Serial.printf("Received message from %u: %s\n", from, msg.c_str());
}

// Callback when a new node connects
void newConnectionCallback(uint32_t nodeId)
{
  Serial.printf("New connection, nodeId = %u\n", nodeId);
}

// Send a message to the mesh network
void sendMessageToMesh(String msg)
{
  mesh.sendBroadcast(msg.c_str()); // Broadcast message to all connected devices
}

/*
sensorDataIn -> input raw sensor data value

decimals -> how many numbers behind the decimal poin should the function return?
(Example: decimals = 1 will result in values such as: 1.5, 81.1 | decimals = 3 will result in values such as: 24.348, 67.157)

readingListLength determines how many readings will be kept and will in turn influence the responsiveness and smoothness. I recommend keeping this as low as possible;
test this and see at what point the processed sensor data stays stable when the lever is not moved. Higher values = smoother but less responsive.
*/
float smoothSensorData(int sensorDataIn, int decimals, int readingListLength) {
    // Static variables to store previous readings and the current index
    static float *readings = nullptr;
    static int index = 0;
    static bool initialized = false;

    // Initialize the readings array only once
    if (!initialized) {
        readings = new float[readingListLength]();
        initialized = true;
    }

    // Add new sensor data to the readings array at the current index
    readings[index] = sensorDataIn;

    // Update the index and wrap around if necessary
    index = (index + 1) % readingListLength;

    // Calculate the sum of all readings in the array
    float sum = 0.0;
    for (int i = 0; i < readingListLength; i++) {
        sum += readings[i];
    }

    // Calculate the average value
    float average = sum / readingListLength;

    // Calculate the rounding factor based on the specified decimal places
    float roundingFactor = pow(10, decimals);

    // Round the average to the specified number of decimal places
    return round(average * roundingFactor) / roundingFactor;
}



void sensorReading()
{
  static unsigned long lastReadTime = 0;
  if (millis() - lastReadTime > (1000 / sensorUpdateRate)) // Run set amount of times per second, default: 60 times per second, to change this: change "sensorUpdateRate" in config.json
  {
    // Read the sensors
    rawSensor0Data = analogRead(sensor0Pin);

    // Process sensor data
    processedSensor0Data = smoothSensorData(rawSensor0Data, 1, sensor0Smoothing);

    sensor0Percentage = map(processedSensor0Data, sensor0MinValue, sensor0MaxValue, 0, 100);

    if (sensor0Percentage != prevSensor0Percentage)
    {
      // Send message containing info to master
      // Serial.println("Sensor data changed");
      
      Serial.println(sensor0Percentage);
    }

    

    // prevRawSensor0Data = rawSensor0Data;
    // prevProcessedSensor0Data = processedSensor0Data;
    prevSensor0Percentage = sensor0Percentage;

    lastReadTime = millis();
  }
}

void setup()
{
  Serial.begin(115200); // Start serial communication

  // Start SPIFFS
  if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED))
  {
    Serial.println("SPIFFS mount failed!");
    return;
  }

  loadConfig(); // Load config from "config.json" in SPIFFS

  // Debug print config info
  Serial.print("Mesh name: ");
  Serial.println(meshName.c_str());

  Serial.print("Mesh password: ");
  Serial.println(meshPassword.c_str());

  Serial.print("Mesh port: ");
  Serial.println(meshPort);

  Serial.print("Device type: ");
  Serial.println(deviceType.c_str());

  Serial.print("Device ID: ");
  Serial.println(deviceID.c_str());

  setupMesh(); // Initialize mesh network

  pinMode(sensor0Pin, INPUT);
}

void loop()
{
  mesh.update(); // Handle mesh tasks

  sensorReading();

  // Send a test message every 10 seconds
  static unsigned long lastSentTime = 0;
  if (millis() - lastSentTime > 10000)
  {
    String message = "Hello from " + deviceType + " " + deviceID;
    sendMessageToMesh(message);
    lastSentTime = millis();
  }
}