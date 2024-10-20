#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>
#include <Update.h>
#include <ArduinoJson.h>
#include <painlessMesh.h>
#include <LoadConfig.h>

#define FORMAT_SPIFFS_IF_FAILED false

unsigned long previousMillis = 0;   // For tracking time between updates
unsigned long updateInterval = 100; // Update interval in ms (0.1 second)

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
void read_sensor_data();

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

void read_sensor_data()
{
  // Placeholder
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