#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>
#include <Update.h>
#include <ArduinoJson.h>
#include <painlessMesh.h>

#define FORMAT_SPIFFS_IF_FAILED false

unsigned long previousMillis = 0;   // For tracking time between updates
unsigned long updateInterval = 100; // Update interval in ms (0.1 second)

/* --- Mesh details --- */
String meshName;
String meshPassword;
String deviceType;
String deviceID;

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
  deviceType = doc["deviceType"].as<String>();     // "slave" or "master"
  deviceID = doc["deviceID"].as<String>();         // Unique device ID
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

  Serial.print("Device type: ");
  Serial.println(deviceType.c_str());

  Serial.print("Device ID: ");
  Serial.println(deviceID.c_str());
}
void loop()
{
}