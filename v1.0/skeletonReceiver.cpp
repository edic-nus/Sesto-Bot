#include <WiFi.h>
#include <WebServer.h>
#include "ArduinoJson.h"

// Replace with your network credentials
const char *ssid = "TP-Link_DE83";
const char *password = "46193346";

// Create a web server on port 80
WebServer server(80);

// Assign output variables to GPIO pins
const int output16 = 16;
const int output17 = 17;
//bool callState1 = false; 
//bool callState2 = false; 

void handleOpenDoor()
{
  digitalWrite(output16, HIGH);
  digitalWrite(output17, LOW);
}

void handleRequest()
{
  if (server.hasArg("plain") == false)
  {
    server.send(400, "application/json", "{\n\t\"status\":\"error\",\n\t\"message\":\"Bad Request - No Data Received\"\n}\n");
    return;
  }

  DynamicJsonDocument doc(1024);
  deserializeJson(doc, server.arg("plain"));
  String action = doc["action"];

  if (action == "openDoor")
  {
    handleOpenDoor();
    server.send(200, "application/json");
    delay(1000);
  } /*else if (action == "buttonStates") {
    if (callState1 == true) {
      String response = "1\n";
      server.send(200, "application/json", response);
      callState1 = false;
      delay(1000);
    } else if (callState2 == true) {
      String response = "2\n";
      server.send(200, "application/json", response);
      callState2 = false;
      delay(1000);
    }
  } else if (action == "changeButton1") {
    server.send(200, "application/json");
    callState1 = true;
    callState2 = false;
    delay(1000);
  } else if (action == "changeButton2") {
    server.send(200, "application/json");
    callState2 = true;
    callState1 = false;
    delay(1000);
  }*/ else {
    server.send(400, "application/json", "{\n\t\"status\":\"error\",\n\t\"data\":{\n\t\t\"message\":\"Invalid Action\"\n\t}\n}\n");
  }
}

void setup()
{
  Serial.begin(115200);

  // Initialize the output variables as outputs
  pinMode(output16, OUTPUT);
  pinMode(output17, OUTPUT);
  // Set outputs to LOW
  digitalWrite(output16, LOW);
  digitalWrite(output17, LOW);

  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();

  Serial.println("Connected to WiFi");

  // Define endpoint and corresponding handler function
  server.on("/request", HTTP_POST, handleRequest);

  // Start the server
  server.begin();
  Serial.println("Server started");
}

void loop()
{
  // Handle client requests
  server.handleClient();
}