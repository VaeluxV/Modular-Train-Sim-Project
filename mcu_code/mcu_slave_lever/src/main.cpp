#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>
#include <Update.h>
#include <ArduinoJson.h>
#include <painlessMesh.h>

#define FORMAT_SPIFFS_IF_FAILED false

unsigned long previousMillis = 0;   // For tracking time between updates
unsigned long updateInterval = 100; // Update interval in ms (0.1 second)

int rawSensorData; // For storing the raw sensor data
int prevRawSensorData; // For storing the previous sensor data

int processedSensorData; // For storing the processed sensor data (example: smoothed out data)
int prevProcessedSensorData; // For storing previous state of processed sensor data. Used for the program to know if it needs to send new sensor data or not

int sensor0Pin;

int sensorUpdateRate; // How many readings per second you want the sensors to take. Default: 120, you can change this in the config.
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
float smoothSensorData();
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

  meshName = doc["meshName"].as<String>();         // "traincabmesh" by default, please change this in the config.json file
  meshPassword = doc["meshPassword"].as<String>(); // "traincabmesh_ABCDEFG123456" by default, please change this in the config.json file
  meshPort = doc["meshPort"].as<int>();            // Port for the mesh network, has to be the same on all devices.
  deviceType = doc["deviceType"].as<String>();     // "slave" or "master"
  deviceID = doc["deviceID"].as<String>();         // Unique device ID
  sensorUpdateRate = doc["sensorUpdateRate"].as<int>(); // Get the sensor update rate (in times per second), 120 by default
  sensor0Pin = doc["sensor0Pin"].as<int>();            // Sensor 0 physical pin connection
}

// Initialize mesh network
void setupMesh() {
  mesh.setDebugMsgTypes(ERROR | STARTUP | CONNECTION);  // Enable debugging
  mesh.init(meshName.c_str(), meshPassword.c_str(), meshPort);  // Initialize mesh with name, password, and port 5555
  mesh.onReceive(&receivedCallback);  // Set callback for receiving messages
  mesh.onNewConnection(&newConnectionCallback);  // Set callback for new connections
}

void receivedCallback(uint32_t from, String &msg) {
  Serial.printf("Received message from %u: %s\n", from, msg.c_str());
}

// Callback when a new node connects
void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("New connection, nodeId = %u\n", nodeId);
}

// Send a message to the mesh network
void sendMessageToMesh(String msg) {
  mesh.sendBroadcast(msg.c_str());  // Broadcast message to all connected devices
}

/*
sensorDataIn -> input raw sensor data value | decimals -> how many numbers behind the decimal poin should the function return? (Example: decimals = 1 will result in values such as: 1.5, 81.1
decimals = 3 will result in values such as: 24.348, 67.157)
smoothingFactor, the higher this is the smoother output will be but it will become less responsive, recommended default: 0.1. Minimum - maximum values: 0 - 1
*/
float smoothSensorData(int sensorDataIn, int decimals, float smoothingFactor) 
{
    // Define a static variable to store the previous smoothed value
    static float smoothedValue = 0.0;

    // Calculate the smoothed value using a basic exponential moving average
    smoothedValue = (sensorDataIn * smoothingFactor) + (smoothedValue * (1.0 - smoothingFactor));

    // Calculate the rounding factor based on the desired decimal places
    float roundingFactor = pow(10, decimals);

    // Return the smoothed value rounded to the specified decimal places
    return round(smoothedValue * roundingFactor) / roundingFactor;
}

void sensorReading()
{
  // Read the sensors
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

  setupMesh();  // Initialize mesh network

  pinMode(sensor0Pin, INPUT);
}

void loop()
{
  mesh.update();  // Handle mesh tasks

  // Send a test message every 10 seconds
  static unsigned long lastSentTime = 0;
  if (millis() - lastSentTime > 10000) {
    String message = "Hello from " + deviceType + " " + deviceID;
    sendMessageToMesh(message);
    lastSentTime = millis();
  }
}