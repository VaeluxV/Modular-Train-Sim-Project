#include <Arduino.h>
#include <painlessMesh.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define MESH_PREFIX "TrainSimControl"
#define MESH_PASSWORD "VerySecurePassword123456"
#define MESH_PORT 6969

Scheduler userScheduler; // to control your personal task
painlessMesh mesh;

String nodeName;
std::map<String, String> nodeData;

void sendMessage(String targetNode, String message);
void receivedCallback(uint32_t from, String &msg);
void newConnectionCallback(uint32_t nodeId);
void changedConnectionCallback();
void nodeTimeAdjustedCallback(int32_t offset);
void setupTask1(void *pvParameters);
void setupTask2(void *pvParameters);

void setup() {
  Serial.begin(9600);

  nodeName = "main"; // Set the name of this node, change as necessary

  mesh.setDebugMsgTypes(ERROR | STARTUP | CONNECTION); // set before init() so that you can see startup messages
  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);

  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  xTaskCreatePinnedToCore(
    setupTask1,     // Function that should be called
    "Task1",        // Name of the task (for debugging)
    10000,          // Stack size (bytes)
    NULL,           // Parameter to pass
    1,              // Task priority
    NULL,           // Task handle
    0               // Core you want to run the task on (0 or 1)
  );

  xTaskCreatePinnedToCore(
    setupTask2,     // Function that should be called
    "Task2",        // Name of the task (for debugging)
    10000,          // Stack size (bytes)
    NULL,           // Parameter to pass
    1,              // Task priority
    NULL,           // Task handle
    1               // Core you want to run the task on (0 or 1)
  );
}

void loop() {
  // it will run the user scheduler as well
  mesh.update();
}

void sendMessage(String targetNode, String message) {
  // Send a message to a specific node identified by `targetNode`
  DynamicJsonDocument doc(1024);
  doc["to"] = targetNode;
  doc["message"] = message;

  String msg;
  serializeJson(doc, msg);
  mesh.sendBroadcast(msg);
}

void receivedCallback(uint32_t from, String &msg) {
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, msg);

  String targetNode = doc["to"];
  String message = doc["message"];

  if (targetNode == nodeName) {
    int start = 0;
    int end = message.indexOf('|');
    while (end != -1) {
      String varPair = message.substring(start, end);
      int separator = varPair.indexOf(':');
      if (separator != -1) {
        String varName = varPair.substring(0, separator);
        String varValue = varPair.substring(separator + 1);
        nodeData[varName] = varValue;
      }
      start = end + 1;
      end = message.indexOf('|', start);
    }
  }
}

void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}

void setupTask1(void *pvParameters) {
  for (;;) {
    // Placeholder for the main task logic
    sendMessage(String("combinedTB"), String("Hi! Test successful!"));

    vTaskDelay(1000 / 120 / portTICK_PERIOD_MS); // Update at 120 times per second
  }
}

void setupTask2(void *pvParameters) {
  for (;;) {
    // Placeholder for the second core task logic
    vTaskDelay(1000 / portTICK_PERIOD_MS); // Just delay to keep the task alive
  }
}
