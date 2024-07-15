#include <WiFi.h>
#include <WebServer.h>
#include "ArduinoJson.h"

// Replace with your network credentials
const char *ssid = "TP-Link_DE83";
const char *password = "46193346";

// Create a web server on port 80
WebServer server(80);

// Assign output variables to GPIO pins
const int output5 = 5;
const int output6 = 6;

void handleOpenLock()
{
  digitalWrite(output5, HIGH);
  delay(200);
  digitalWrite(output5, LOW);
  delay(200);
}

void handleCloseLock()
{
  digitalWrite(output6, HIGH);
  delay(200);
  digitalWrite(output6, LOW);
  delay(200);

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

  if (action == "open")
  {
    handleOpenLock();
    server.send(200, "application/json");
    delay(1000);
  }
  else if (action == "close")
  {
    handleCloseLock();
    server.send(200, "application/json");
    delay(1000);
  }
  else {
    server.send(400, "application/json", "{\n\t\"status\":\"error\",\n\t\"data\":{\n\t\t\"message\":\"Invalid Action\"\n\t}\n}\n");
  }
}

void setup()
{
  Serial.begin(115200);

  // Initialize the output variables as outputs
  pinMode(output5, OUTPUT);
  pinMode(output6, OUTPUT);

  // Set outputs to LOW
  digitalWrite(output5, LOW);
  digitalWrite(output6, LOW);

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
