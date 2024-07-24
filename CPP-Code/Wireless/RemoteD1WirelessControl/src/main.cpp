#include <Arduino.h>
#include <painlessMesh.h>
#include <ArduinoJson.h>

#define MESH_SSID "TrainSimCabMeshNet" // Set your mesh network name
#define MESH_PASSWORD "SecurePassword123" // Set your mesh password
#define MESH_PORT 5555 // Port for communication, default is 5555

#define NODE_NAME "node_combined_throttle_brake"

// Create an instance of the painlessMesh object
painlessMesh mesh;

void setup() {
    Serial.begin(9600); // Initialize serial communication
    Serial.println("Starting Mesh Network...");

    // Initialize mesh
    mesh.init(MESH_SSID, MESH_PASSWORD, MESH_PORT);
    
    // Set the node name (optional)
    mesh.setHostname(NODE_NAME); // Set the node name
    
    // Callback when a message is received
    mesh.onReceive([](uint32_t from, String &msg) {
        Serial.printf("Message received from node %u: %s\n", from, msg.c_str());
    });

    // Start periodic message sending
    mesh.onNewConnection([](uint32_t nodeId) {
        Serial.printf("New connection established with node %u\n", nodeId);
    });

    mesh.onDroppedConnection([](uint32_t nodeId) {
        Serial.printf("Dropped connection with node %u\n", nodeId);
    });
}

void loop() {
    mesh.update(); // Keep the mesh network updated

    // Send a message every 16 milliseconds (about 60 times per second)
    static unsigned long lastSendTime = 0;
    unsigned long currentTime = millis();
    
    if (currentTime - lastSendTime >= 16) {
        String message = "Hello Mesh!";
        mesh.sendBroadcast(message); // Send message to all nodes in the mesh
        lastSendTime = currentTime;
        Serial.println("Message sent: " + message);
    }

    // You can adjust the message frequency here if needed
    // For 120 messages per second, use 8 milliseconds
}
