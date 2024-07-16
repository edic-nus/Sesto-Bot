#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include "ArduinoJson.h"
#include <Arduino_JSON.h>

const char* ssid = "TP-Link_DE83";
const char* password = "46193346";

const char* serverName = "http://192.168.0.104/request";

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Server started");

  WiFiClient client;
  HTTPClient http;
  http.begin(client, serverName);
  http.addHeader("Content-Type", "application/json");

  // Create JSON content
  DynamicJsonDocument doc(1024);
  doc["action"] = "close";
  String jsonString;
  serializeJson(doc, jsonString);

  // Send HTTP POST request
  int httpResponseCode = http.POST(jsonString);

  // Print the response
  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println(httpResponseCode);   // HTTP response code
    Serial.println(response);           // HTTP response
  } else {
    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode);
  }


  int httpResponseCode = http.POST();
  http.end();
}
