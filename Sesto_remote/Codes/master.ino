#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include "ArduinoJson.h"
#include <Arduino_JSON.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Wire.h"

#define SCREEN_I2C_ADDR 0x3C  // or 0x3C
#define SCREEN_WIDTH 128      // OLED display width, in pixels
#define SCREEN_HEIGHT 64      // OLED display height, in pixels
#define OLED_RST_PIN -1       // Reset pin (-1 if not available)

int I2C_SDA_PIN = 4;
int I2C_SCL_PIN = 5;

Adafruit_SSD1306 display(128, 64, &Wire, OLED_RST_PIN);

#define FRAME_DELAY (10)
#define FRAME_WIDTH (32)
#define FRAME_HEIGHT (32)
#define FRAME_COUNT (28)
#define UI_MIDDLE_X (48)
#define UI_MIDDLE_Y (10)

bool power_on = false;         // power on/off state of the remote
const int batteryPin = 6;      // ADC pin connected to the voltage divider
float res1 = 10000.0;          // Resistance of R1 (in ohms)
float res2 = 10000.0;          // Resistance of R2 (in ohms)
float maxAdcValue = 4095.0;    // Maximum value for the 12-bit ADC
float referenceVoltage = 3.3;  // Reference voltage for the ESP32 ADC

const char* ssid = "TP-Link_DE83";
const char* password = "46193346";
WebServer server(80);

const char* cabinetserverName = "http://192.168.0.104/request";
const char* sestoinfoServerName = "http://192.168.0.100/public/amrs/statuses";
const char* requestServerName = "http://192.168.0.100/public/tasks";
String postServerName = "http://192.168.0.100/public/tasks";


// define all the buttons
// Button pins
int R1 = 42;
int R2 = 41;
int R3 = 40;
int C1 = 39;
int C2 = 38;
int C3 = 37;

byte rows[] = { R1, R2, R3 };
const int rowCount = sizeof(rows) / sizeof(rows[0]);

// columns
byte cols[] = { C1, C2, C3 };
const int colCount = sizeof(cols) / sizeof(cols[0]);

// array
byte keys[colCount][rowCount];

String retrieve_botstatus() {
  // retrieves information from sestobot
  WiFiClient client;
  HTTPClient http;
  http.begin(client, sestoinfoServerName);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "Bearer eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJpYXQiOjE3MTgxMjQ1MjcsIm5iZiI6MTcxODEyNDUyNywianRpIjoiNmVhYWY4MmItYzg1NC00MjNmLTk1ZDktOTMwNGYyZjVjOGRhIiwiZXhwIjoxNzIwNzE2NTI3LCJpZGVudGl0eSI6eyJncm91cF9uYW1lIjoicGFydG5lcl9hZG1pbiIsInVzZXJfbmFtZSI6InBhcnRuZXIiLCJmaXJzdF9uYW1lIjoiRElTVFJJQlVUT1IiLCJsYXN0X25hbWUiOiJBZG1pbiIsImVtYWlsIjoiIiwiY3JlYXRvciI6InNlc3RvIiwiaWQiOiJjZGFmYjc2Ni04OGMxLTQwYTktYjc2NC0xMWRjYTc5MGNkNDkiLCJub3RpZmljYXRpb25fc3Vic2NyaWJlcnMiOnsiYWxlcnQiOnsidWkiOnRydWUsImVtYWlsIjpmYWxzZX19LCJpc19lbWFpbF9vdHBfc2VudCI6ZmFsc2UsImlzX2VtYWlsX3ZlcmlmaWVkIjpmYWxzZX0sImZyZXNoIjpmYWxzZSwidHlwZSI6ImFjY2VzcyJ9.GSR2u5W-wVDyRdMmXZBwzd92hZnHS77_mwEwQV38Y9U");
  int httpResponseCode = http.GET();
  String response = http.getString();
  JSONVar object = JSON.parse(response);
  http.end();

  // retrieves task ID from JSON file
  String id_data = object[0]["status"];
  return id_data;  // the dictionary information from the SESTO bot
}

int retrieve_workid() {
  // retrieves information from sestobot
  WiFiClient client;
  HTTPClient http;
  http.begin(client, sestoinfoServerName);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "Bearer eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJpYXQiOjE3MTgxMjQ1MjcsIm5iZiI6MTcxODEyNDUyNywianRpIjoiNmVhYWY4MmItYzg1NC00MjNmLTk1ZDktOTMwNGYyZjVjOGRhIiwiZXhwIjoxNzIwNzE2NTI3LCJpZGVudGl0eSI6eyJncm91cF9uYW1lIjoicGFydG5lcl9hZG1pbiIsInVzZXJfbmFtZSI6InBhcnRuZXIiLCJmaXJzdF9uYW1lIjoiRElTVFJJQlVUT1IiLCJsYXN0X25hbWUiOiJBZG1pbiIsImVtYWlsIjoiIiwiY3JlYXRvciI6InNlc3RvIiwiaWQiOiJjZGFmYjc2Ni04OGMxLTQwYTktYjc2NC0xMWRjYTc5MGNkNDkiLCJub3RpZmljYXRpb25fc3Vic2NyaWJlcnMiOnsiYWxlcnQiOnsidWkiOnRydWUsImVtYWlsIjpmYWxzZX19LCJpc19lbWFpbF9vdHBfc2VudCI6ZmFsc2UsImlzX2VtYWlsX3ZlcmlmaWVkIjpmYWxzZX0sImZyZXNoIjpmYWxzZSwidHlwZSI6ImFjY2VzcyJ9.GSR2u5W-wVDyRdMmXZBwzd92hZnHS77_mwEwQV38Y9U");
  int httpResponseCode = http.GET();
  String response = http.getString();
  JSONVar object = JSON.parse(response);
  http.end();

  // retrieves task ID from JSON file
  int id_data = object[0]["id"];
  return id_data;  // the dictionary information from the SESTO bot
}

int poster(int id_code, int operation) {
  WiFiClient client;
  HTTPClient http;
  Serial.print("configuring workflow id: ");
  Serial.println(id_code);
  String id_num = String(id_code);

  // cancels the tasks, returns bot to nearest parking spot
  if (operation == 0) {
    http.begin(client, postServerName + "cancel?id=" + id_num);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", "Bearer eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJpYXQiOjE3MTgxMjQ1MjcsIm5iZiI6MTcxODEyNDUyNywianRpIjoiNmVhYWY4MmItYzg1NC00MjNmLTk1ZDktOTMwNGYyZjVjOGRhIiwiZXhwIjoxNzIwNzE2NTI3LCJpZGVudGl0eSI6eyJncm91cF9uYW1lIjoicGFydG5lcl9hZG1pbiIsInVzZXJfbmFtZSI6InBhcnRuZXIiLCJmaXJzdF9uYW1lIjoiRElTVFJJQlVUT1IiLCJsYXN0X25hbWUiOiJBZG1pbiIsImVtYWlsIjoiIiwiY3JlYXRvciI6InNlc3RvIiwiaWQiOiJjZGFmYjc2Ni04OGMxLTQwYTktYjc2NC0xMWRjYTc5MGNkNDkiLCJub3RpZmljYXRpb25fc3Vic2NyaWJlcnMiOnsiYWxlcnQiOnsidWkiOnRydWUsImVtYWlsIjpmYWxzZX19LCJpc19lbWFpbF9vdHBfc2VudCI6ZmFsc2UsImlzX2VtYWlsX3ZlcmlmaWVkIjpmYWxzZX0sImZyZXNoIjpmYWxzZSwidHlwZSI6ImFjY2VzcyJ9.GSR2u5W-wVDyRdMmXZBwzd92hZnHS77_mwEwQV38Y9U");
    int httpResponseCode = http.POST("{\"id\":" + id_num + "}");
    http.end();
    return httpResponseCode;
  }

  // pauses the tasks, bot stays on the spot
  if (operation == 1) {
    Serial.println("Pausing...");
    http.begin(client, postServerName + "pause?id=" + id_num);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", "Bearer eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJpYXQiOjE3MTgxMjQ1MjcsIm5iZiI6MTcxODEyNDUyNywianRpIjoiNmVhYWY4MmItYzg1NC00MjNmLTk1ZDktOTMwNGYyZjVjOGRhIiwiZXhwIjoxNzIwNzE2NTI3LCJpZGVudGl0eSI6eyJncm91cF9uYW1lIjoicGFydG5lcl9hZG1pbiIsInVzZXJfbmFtZSI6InBhcnRuZXIiLCJmaXJzdF9uYW1lIjoiRElTVFJJQlVUT1IiLCJsYXN0X25hbWUiOiJBZG1pbiIsImVtYWlsIjoiIiwiY3JlYXRvciI6InNlc3RvIiwiaWQiOiJjZGFmYjc2Ni04OGMxLTQwYTktYjc2NC0xMWRjYTc5MGNkNDkiLCJub3RpZmljYXRpb25fc3Vic2NyaWJlcnMiOnsiYWxlcnQiOnsidWkiOnRydWUsImVtYWlsIjpmYWxzZX19LCJpc19lbWFpbF9vdHBfc2VudCI6ZmFsc2UsImlzX2VtYWlsX3ZlcmlmaWVkIjpmYWxzZX0sImZyZXNoIjpmYWxzZSwidHlwZSI6ImFjY2VzcyJ9.GSR2u5W-wVDyRdMmXZBwzd92hZnHS77_mwEwQV38Y9U");
    int httpResponseCode = http.POST("{\"id\":" + id_num + "}");
    http.end();
    return httpResponseCode;
  }

  // resumes the tasks
  if (operation == 2) {
    Serial.println("Resuming...");
    http.begin(client, postServerName + "resume?id=" + id_num);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", "Bearer eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJpYXQiOjE3MTgxMjQ1MjcsIm5iZiI6MTcxODEyNDUyNywianRpIjoiNmVhYWY4MmItYzg1NC00MjNmLTk1ZDktOTMwNGYyZjVjOGRhIiwiZXhwIjoxNzIwNzE2NTI3LCJpZGVudGl0eSI6eyJncm91cF9uYW1lIjoicGFydG5lcl9hZG1pbiIsInVzZXJfbmFtZSI6InBhcnRuZXIiLCJmaXJzdF9uYW1lIjoiRElTVFJJQlVUT1IiLCJsYXN0X25hbWUiOiJBZG1pbiIsImVtYWlsIjoiIiwiY3JlYXRvciI6InNlc3RvIiwiaWQiOiJjZGFmYjc2Ni04OGMxLTQwYTktYjc2NC0xMWRjYTc5MGNkNDkiLCJub3RpZmljYXRpb25fc3Vic2NyaWJlcnMiOnsiYWxlcnQiOnsidWkiOnRydWUsImVtYWlsIjpmYWxzZX19LCJpc19lbWFpbF9vdHBfc2VudCI6ZmFsc2UsImlzX2VtYWlsX3ZlcmlmaWVkIjpmYWxzZX0sImZyZXNoIjpmYWxzZSwidHlwZSI6ImFjY2VzcyJ9.GSR2u5W-wVDyRdMmXZBwzd92hZnHS77_mwEwQV38Y9U");
    int httpResponseCode = http.POST("{\"id\":" + id_num + "}");
    http.end();
    return httpResponseCode;
  } 
}

int request_tinkering_corner() {
  WiFiClient client;
  HTTPClient http;
  http.begin(client, requestServerName);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "Bearer eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJpYXQiOjE3MTgxMjQ1MjcsIm5iZiI6MTcxODEyNDUyNywianRpIjoiNmVhYWY4MmItYzg1NC00MjNmLTk1ZDktOTMwNGYyZjVjOGRhIiwiZXhwIjoxNzIwNzE2NTI3LCJpZGVudGl0eSI6eyJncm91cF9uYW1lIjoicGFydG5lcl9hZG1pbiIsInVzZXJfbmFtZSI6InBhcnRuZXIiLCJmaXJzdF9uYW1lIjoiRElTVFJJQlVUT1IiLCJsYXN0X25hbWUiOiJBZG1pbiIsImVtYWlsIjoiIiwiY3JlYXRvciI6InNlc3RvIiwiaWQiOiJjZGFmYjc2Ni04OGMxLTQwYTktYjc2NC0xMWRjYTc5MGNkNDkiLCJub3RpZmljYXRpb25fc3Vic2NyaWJlcnMiOnsiYWxlcnQiOnsidWkiOnRydWUsImVtYWlsIjpmYWxzZX19LCJpc19lbWFpbF9vdHBfc2VudCI6ZmFsc2UsImlzX2VtYWlsX3ZlcmlmaWVkIjpmYWxzZX0sImZyZXNoIjpmYWxzZSwidHlwZSI6ImFjY2VzcyJ9.GSR2u5W-wVDyRdMmXZBwzd92hZnHS77_mwEwQV38Y9U");
  int httpResponseCode = http.POST("{\"amr_id\":15,\"workflow_id\":146}");
  http.end();
  return httpResponseCode;
}

int request_sandbox() {
  WiFiClient client;
  HTTPClient http;
  http.begin(client, requestServerName);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "Bearer eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJpYXQiOjE3MTgxMjQ1MjcsIm5iZiI6MTcxODEyNDUyNywianRpIjoiNmVhYWY4MmItYzg1NC00MjNmLTk1ZDktOTMwNGYyZjVjOGRhIiwiZXhwIjoxNzIwNzE2NTI3LCJpZGVudGl0eSI6eyJncm91cF9uYW1lIjoicGFydG5lcl9hZG1pbiIsInVzZXJfbmFtZSI6InBhcnRuZXIiLCJmaXJzdF9uYW1lIjoiRElTVFJJQlVUT1IiLCJsYXN0X25hbWUiOiJBZG1pbiIsImVtYWlsIjoiIiwiY3JlYXRvciI6InNlc3RvIiwiaWQiOiJjZGFmYjc2Ni04OGMxLTQwYTktYjc2NC0xMWRjYTc5MGNkNDkiLCJub3RpZmljYXRpb25fc3Vic2NyaWJlcnMiOnsiYWxlcnQiOnsidWkiOnRydWUsImVtYWlsIjpmYWxzZX19LCJpc19lbWFpbF9vdHBfc2VudCI6ZmFsc2UsImlzX2VtYWlsX3ZlcmlmaWVkIjpmYWxzZX0sImZyZXNoIjpmYWxzZSwidHlwZSI6ImFjY2VzcyJ9.GSR2u5W-wVDyRdMmXZBwzd92hZnHS77_mwEwQV38Y9U");
  int httpResponseCode = http.POST("{\"amr_id\":15,\"workflow_id\":143}");
  http.end();
  return httpResponseCode;
}

int opencabinet() {
  WiFiClient client;
  HTTPClient http;
  http.begin(client, cabinetserverName);
  http.addHeader("Content-Type", "application/json");

  DynamicJsonDocument doc(1024);
  doc["action"] = "open";
  String jsonString;
  serializeJson(doc, jsonString);

  int httpResponseCode = http.POST(jsonString);
    // Print the response
  return httpResponseCode;
}

void handleRequest() {
  if (server.hasArg("plain") == false) {
    server.send(400, "application/json", "{\n\t\"status\":\"error\",\n\t\"message\":\"Bad Request - No Data Received\"\n}\n");
    return;
  }

  DynamicJsonDocument doc(1024);
  deserializeJson(doc, server.arg("plain"));
  String action = doc["action"];
  String parameter = doc["parameter"];

  if (action == "botStatus") {
    if (parameter == "front") {
      delay(200);
      server.send(200, "application/json");
    } else if (parameter == "back") {
      delay(200);
      server.send(200, "application/json");
    } else if (parameter == "notMoving") {
      delay(200);
      server.send(200, "application/json");
    } else {
      server.send(400, "application/json", "{\n\t\"status\":\"error\",\n\t\"data\":{\n\t\t\"message\":\"Invalid Parameter\"\n\t}\n}\n");
    }
  } else if (action == "stop") {
    delay(200);
    server.send(200, "application/json");
  } else {
    server.send(400, "application/json", "{\n\t\"status\":\"error\",\n\t\"data\":{\n\t\t\"message\":\"Invalid Action\"\n\t}\n}\n");
  }
}

// battery checker
float battery_check_remote() {
  analogReadResolution(12);  // Set ADC resolution to 12 bits
  int adcValue = analogRead(batteryPin);
  float voltage = (adcValue / maxAdcValue) * referenceVoltage;
  float batteryVoltage = voltage * (res1 + res2) / (res2);  // Calculate actual battery voltage
  return batteryVoltage;
}

// defining the animated icons
// each UI icon will have a pos: 0 = right, 1 = middle, 2 = right (except WiFi)
void UI_wifi() {
  const byte PROGMEM frames[][128] = {
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 128, 0, 0, 63, 254, 0, 1, 224, 15, 192, 7, 0, 1, 240, 28, 3, 224, 56, 48, 63, 254, 28, 97, 224, 7, 135, 195, 0, 1, 227, 110, 7, 240, 118, 56, 63, 254, 60, 16, 224, 15, 8, 1, 128, 1, 192, 7, 7, 240, 192, 3, 30, 252, 192, 1, 240, 15, 128, 0, 192, 7, 0, 0, 7, 224, 0, 0, 28, 120, 0, 0, 24, 24, 0, 0, 12, 48, 0, 0, 6, 96, 0, 0, 3, 192, 0, 0, 1, 192, 0, 0, 1, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 128, 0, 0, 63, 254, 0, 1, 224, 15, 192, 7, 0, 1, 240, 28, 3, 224, 56, 48, 63, 254, 28, 97, 224, 7, 135, 195, 0, 1, 227, 110, 7, 240, 118, 56, 63, 254, 60, 16, 224, 15, 8, 1, 128, 1, 192, 7, 7, 240, 192, 3, 30, 252, 192, 1, 240, 15, 128, 0, 192, 7, 0, 0, 7, 224, 0, 0, 28, 120, 0, 0, 24, 24, 0, 0, 12, 48, 0, 0, 6, 96, 0, 0, 3, 192, 0, 0, 1, 192, 0, 0, 1, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 128, 0, 0, 63, 254, 0, 1, 224, 15, 192, 7, 0, 1, 240, 28, 3, 224, 56, 48, 63, 254, 28, 97, 224, 7, 135, 195, 0, 1, 227, 110, 7, 240, 118, 56, 63, 254, 60, 16, 224, 15, 8, 1, 128, 1, 192, 7, 7, 240, 192, 3, 30, 252, 192, 1, 240, 15, 128, 0, 192, 7, 0, 0, 7, 224, 0, 0, 28, 120, 0, 0, 24, 24, 0, 0, 12, 48, 0, 0, 6, 96, 0, 0, 3, 192, 0, 0, 1, 192, 0, 0, 1, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 128, 0, 0, 63, 254, 0, 1, 224, 15, 192, 7, 0, 1, 240, 28, 3, 224, 56, 48, 63, 254, 28, 97, 224, 7, 135, 195, 0, 1, 227, 110, 7, 240, 118, 56, 63, 254, 60, 16, 224, 15, 8, 1, 128, 1, 192, 7, 7, 240, 192, 3, 30, 252, 192, 1, 240, 15, 128, 0, 192, 7, 0, 0, 7, 224, 0, 0, 28, 120, 0, 0, 24, 24, 0, 0, 12, 48, 0, 0, 6, 96, 0, 0, 3, 192, 0, 0, 1, 192, 0, 0, 1, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 128, 0, 0, 63, 254, 0, 1, 224, 15, 192, 7, 0, 1, 240, 28, 3, 224, 56, 48, 63, 254, 28, 97, 224, 7, 135, 195, 0, 1, 227, 110, 31, 248, 118, 56, 120, 30, 60, 17, 192, 7, 136, 3, 128, 129, 192, 3, 31, 248, 224, 3, 184, 61, 192, 1, 224, 7, 128, 0, 195, 227, 0, 0, 15, 240, 0, 0, 24, 24, 0, 0, 28, 56, 0, 0, 12, 112, 0, 0, 6, 96, 0, 0, 3, 192, 0, 0, 1, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 252, 0, 0, 252, 127, 0, 3, 128, 3, 224, 14, 0, 0, 112, 24, 31, 252, 28, 112, 248, 63, 14, 195, 128, 3, 199, 102, 31, 248, 230, 60, 120, 63, 60, 25, 192, 7, 152, 3, 128, 129, 192, 3, 15, 248, 224, 3, 56, 60, 192, 1, 224, 7, 128, 0, 192, 131, 0, 0, 7, 240, 0, 0, 28, 56, 0, 0, 24, 24, 0, 0, 12, 48, 0, 0, 6, 96, 0, 0, 3, 192, 0, 0, 1, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 63, 254, 0, 1, 240, 31, 128, 7, 128, 1, 224, 28, 0, 128, 120, 56, 63, 252, 28, 96, 224, 15, 134, 195, 128, 3, 195, 102, 15, 240, 118, 60, 63, 254, 60, 16, 224, 15, 24, 3, 128, 1, 192, 7, 7, 224, 224, 3, 30, 252, 192, 1, 240, 15, 128, 0, 192, 7, 0, 0, 0, 0, 0, 0, 7, 224, 0, 0, 14, 120, 0, 0, 24, 24, 0, 0, 12, 48, 0, 0, 6, 96, 0, 0, 7, 224, 0, 0, 3, 192, 0, 0, 1, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 252, 0, 0, 252, 127, 128, 3, 128, 3, 224, 14, 0, 0, 120, 56, 31, 252, 28, 112, 248, 63, 14, 195, 192, 3, 195, 230, 0, 0, 231, 124, 0, 0, 62, 56, 0, 0, 28, 0, 0, 0, 0, 0, 1, 192, 0, 0, 31, 252, 0, 0, 112, 14, 0, 0, 192, 135, 128, 1, 143, 241, 128, 0, 248, 31, 0, 0, 96, 14, 0, 0, 7, 240, 0, 0, 28, 56, 0, 0, 24, 24, 0, 0, 12, 48, 0, 0, 6, 96, 0, 0, 3, 192, 0, 0, 1, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 192, 0, 0, 63, 252, 0, 0, 224, 15, 128, 3, 131, 225, 192, 6, 31, 252, 96, 12, 224, 15, 48, 7, 128, 3, 240, 7, 0, 0, 224, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 224, 0, 0, 13, 240, 0, 0, 31, 248, 0, 0, 24, 56, 0, 0, 24, 24, 0, 0, 12, 48, 0, 0, 6, 96, 0, 0, 3, 192, 0, 0, 1, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 224, 0, 0, 10, 184, 0, 0, 54, 252, 0, 0, 31, 248, 0, 0, 28, 120, 0, 0, 24, 24, 0, 0, 12, 48, 0, 0, 6, 96, 0, 0, 3, 192, 0, 0, 1, 128, 0, 0, 0, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 192, 0, 0, 15, 240, 0, 0, 24, 56, 0, 0, 24, 24, 0, 0, 12, 48, 0, 0, 6, 96, 0, 0, 3, 192, 0, 0, 1, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 224, 0, 0, 28, 120, 0, 0, 24, 24, 0, 0, 12, 48, 0, 0, 6, 96, 0, 0, 3, 192, 0, 0, 1, 192, 0, 0, 1, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 224, 0, 0, 28, 120, 0, 0, 24, 24, 0, 0, 12, 48, 0, 0, 6, 96, 0, 0, 3, 192, 0, 0, 1, 192, 0, 0, 1, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 224, 0, 0, 28, 120, 0, 0, 24, 24, 0, 0, 12, 48, 0, 0, 6, 96, 0, 0, 3, 192, 0, 0, 1, 192, 0, 0, 1, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 224, 0, 0, 28, 120, 0, 0, 24, 24, 0, 0, 12, 48, 0, 0, 6, 96, 0, 0, 3, 192, 0, 0, 1, 192, 0, 0, 1, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 224, 0, 0, 28, 120, 0, 0, 24, 24, 0, 0, 12, 48, 0, 0, 6, 96, 0, 0, 3, 192, 0, 0, 1, 192, 0, 0, 1, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 224, 0, 0, 28, 120, 0, 0, 24, 24, 0, 0, 12, 48, 0, 0, 6, 96, 0, 0, 3, 192, 0, 0, 1, 192, 0, 0, 1, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 224, 0, 0, 28, 120, 0, 0, 24, 24, 0, 0, 12, 48, 0, 0, 6, 96, 0, 0, 3, 192, 0, 0, 1, 192, 0, 0, 1, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 224, 0, 0, 13, 248, 0, 0, 31, 248, 0, 0, 24, 56, 0, 0, 24, 24, 0, 0, 12, 48, 0, 0, 6, 96, 0, 0, 3, 192, 0, 0, 1, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 240, 0, 0, 60, 62, 0, 0, 193, 199, 0, 1, 159, 249, 192, 3, 112, 14, 192, 1, 192, 7, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 240, 0, 0, 28, 120, 0, 0, 24, 24, 0, 0, 12, 48, 0, 0, 6, 96, 0, 0, 3, 192, 0, 0, 1, 128, 0, 0, 0, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 248, 0, 0, 126, 255, 0, 3, 192, 3, 192, 6, 1, 192, 224, 28, 63, 252, 56, 56, 240, 15, 28, 51, 128, 3, 204, 31, 0, 0, 248, 12, 0, 0, 48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 224, 0, 0, 30, 248, 0, 0, 112, 14, 0, 0, 103, 246, 0, 0, 61, 188, 0, 0, 31, 248, 0, 0, 24, 56, 0, 0, 24, 24, 0, 0, 12, 48, 0, 0, 6, 96, 0, 0, 3, 192, 0, 0, 1, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 252, 0, 0, 252, 127, 128, 3, 128, 3, 224, 14, 0, 0, 120, 56, 31, 252, 28, 112, 248, 63, 14, 195, 192, 3, 195, 230, 0, 0, 231, 124, 0, 0, 62, 56, 0, 0, 28, 0, 15, 240, 0, 0, 62, 254, 0, 0, 224, 15, 0, 1, 129, 131, 128, 3, 143, 249, 192, 1, 248, 31, 128, 0, 224, 15, 0, 0, 0, 0, 0, 0, 7, 240, 0, 0, 28, 56, 0, 0, 24, 24, 0, 0, 12, 48, 0, 0, 6, 96, 0, 0, 3, 192, 0, 0, 1, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 63, 254, 0, 1, 240, 31, 128, 7, 128, 1, 224, 28, 0, 128, 120, 56, 63, 252, 28, 96, 224, 15, 134, 195, 128, 3, 195, 102, 15, 240, 118, 60, 63, 254, 60, 16, 224, 15, 24, 3, 128, 1, 192, 7, 7, 224, 224, 3, 30, 252, 192, 1, 240, 15, 128, 0, 192, 7, 0, 0, 0, 0, 0, 0, 7, 224, 0, 0, 14, 120, 0, 0, 24, 24, 0, 0, 12, 48, 0, 0, 6, 96, 0, 0, 7, 224, 0, 0, 3, 192, 0, 0, 1, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 252, 0, 0, 252, 127, 0, 3, 128, 3, 224, 14, 0, 0, 112, 24, 31, 252, 28, 112, 248, 63, 14, 195, 128, 3, 199, 102, 31, 248, 230, 60, 120, 63, 60, 25, 192, 7, 152, 3, 128, 129, 192, 3, 15, 248, 224, 3, 56, 60, 192, 1, 224, 7, 128, 0, 192, 131, 0, 0, 7, 240, 0, 0, 28, 56, 0, 0, 24, 24, 0, 0, 12, 48, 0, 0, 6, 96, 0, 0, 3, 192, 0, 0, 1, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 128, 0, 0, 63, 254, 0, 1, 224, 15, 192, 7, 0, 1, 240, 28, 3, 224, 56, 48, 63, 254, 28, 97, 224, 7, 135, 195, 0, 1, 227, 110, 31, 248, 118, 56, 120, 30, 60, 17, 192, 7, 136, 3, 128, 129, 192, 3, 31, 248, 224, 3, 184, 61, 192, 1, 224, 7, 128, 0, 195, 227, 0, 0, 15, 240, 0, 0, 24, 24, 0, 0, 28, 56, 0, 0, 12, 112, 0, 0, 6, 96, 0, 0, 3, 192, 0, 0, 1, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 128, 0, 0, 63, 254, 0, 1, 224, 15, 192, 7, 0, 1, 240, 28, 3, 224, 56, 48, 63, 254, 28, 97, 224, 7, 135, 195, 0, 1, 227, 110, 7, 240, 118, 56, 63, 254, 60, 16, 224, 15, 8, 1, 128, 1, 192, 7, 7, 240, 192, 3, 30, 252, 192, 1, 240, 15, 128, 0, 192, 7, 0, 0, 7, 224, 0, 0, 28, 120, 0, 0, 24, 24, 0, 0, 12, 48, 0, 0, 6, 96, 0, 0, 3, 192, 0, 0, 1, 192, 0, 0, 1, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 128, 0, 0, 63, 254, 0, 1, 224, 15, 192, 7, 0, 1, 240, 28, 3, 224, 56, 48, 63, 254, 28, 97, 224, 7, 135, 195, 0, 1, 227, 110, 7, 240, 118, 56, 63, 254, 60, 16, 224, 15, 8, 1, 128, 1, 192, 7, 7, 240, 192, 3, 30, 252, 192, 1, 240, 15, 128, 0, 192, 7, 0, 0, 7, 224, 0, 0, 28, 120, 0, 0, 24, 24, 0, 0, 12, 48, 0, 0, 6, 96, 0, 0, 3, 192, 0, 0, 1, 192, 0, 0, 1, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 128, 0, 0, 63, 254, 0, 1, 224, 15, 192, 7, 0, 1, 240, 28, 3, 224, 56, 48, 63, 254, 28, 97, 224, 7, 135, 195, 0, 1, 227, 110, 7, 240, 118, 56, 63, 254, 60, 16, 224, 15, 8, 1, 128, 1, 192, 7, 7, 240, 192, 3, 30, 252, 192, 1, 240, 15, 128, 0, 192, 7, 0, 0, 7, 224, 0, 0, 28, 120, 0, 0, 24, 24, 0, 0, 12, 48, 0, 0, 6, 96, 0, 0, 3, 192, 0, 0, 1, 192, 0, 0, 1, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
  };
  display.fillRoundRect(0, 53, 128, 12, 3, 1);
  display.setTextColor(SSD1306_BLACK);
  display.setTextSize(1);
  display.setCursor(1, 55);
  display.println(F("Connecting to WiFi..."));
  for (int frame = 0; frame < 28; frame++) {
    display.fillRoundRect(UI_MIDDLE_X - 4, UI_MIDDLE_Y - 4, 40, 40, 10, 0);
    display.drawBitmap(UI_MIDDLE_X, UI_MIDDLE_Y, frames[frame], FRAME_WIDTH, FRAME_HEIGHT, 1);
    delay(50);
    display.display();
  }
}

void UI_battery(int pos) {
  const byte PROGMEM frames[][128] = {
    { 0, 0, 0, 0, 0, 7, 240, 0, 0, 15, 240, 0, 1, 252, 31, 192, 1, 252, 31, 192, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 255, 255, 192, 1, 255, 255, 192, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 7, 240, 0, 0, 15, 240, 0, 1, 252, 31, 192, 1, 252, 31, 192, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 255, 255, 192, 1, 255, 255, 192, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 7, 240, 0, 0, 15, 240, 0, 1, 252, 31, 192, 1, 252, 31, 192, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 255, 255, 192, 1, 255, 255, 192, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 7, 240, 0, 0, 15, 240, 0, 1, 252, 31, 192, 1, 252, 31, 192, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 255, 255, 192, 1, 255, 255, 192, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 7, 240, 0, 0, 15, 240, 0, 1, 252, 31, 192, 1, 252, 31, 192, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 128, 64, 1, 0, 128, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 255, 255, 192, 1, 255, 255, 192, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 7, 240, 0, 0, 15, 240, 0, 1, 252, 31, 192, 1, 252, 31, 192, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 128, 64, 1, 0, 128, 64, 1, 0, 192, 64, 1, 1, 192, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 255, 255, 192, 1, 255, 255, 192, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 7, 240, 0, 0, 15, 240, 0, 1, 252, 31, 192, 1, 252, 31, 192, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 64, 64, 1, 0, 64, 64, 1, 0, 64, 64, 1, 0, 104, 64, 1, 0, 120, 64, 1, 0, 248, 64, 1, 0, 248, 64, 1, 0, 56, 64, 1, 0, 24, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 255, 255, 192, 1, 255, 255, 192, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 7, 240, 0, 0, 15, 240, 0, 1, 252, 31, 192, 1, 252, 31, 192, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 64, 64, 1, 0, 64, 64, 1, 0, 64, 64, 1, 0, 192, 64, 1, 0, 192, 64, 1, 1, 128, 64, 1, 1, 240, 64, 1, 3, 240, 64, 1, 3, 240, 64, 1, 3, 224, 64, 1, 0, 96, 64, 1, 0, 192, 64, 1, 0, 192, 64, 1, 0, 128, 64, 1, 0, 128, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 255, 255, 192, 1, 255, 255, 192, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 7, 240, 0, 0, 15, 240, 0, 1, 252, 31, 192, 1, 252, 31, 192, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 128, 64, 1, 0, 128, 64, 1, 1, 128, 64, 1, 1, 128, 64, 1, 3, 128, 64, 1, 3, 240, 64, 1, 3, 224, 64, 1, 7, 224, 64, 1, 7, 192, 64, 1, 0, 192, 64, 1, 0, 192, 64, 1, 0, 128, 64, 1, 1, 128, 64, 1, 1, 0, 64, 1, 1, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 255, 255, 192, 1, 255, 255, 192, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 7, 240, 0, 0, 15, 240, 0, 1, 252, 31, 192, 1, 252, 31, 192, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 128, 64, 1, 0, 0, 64, 1, 1, 0, 64, 1, 1, 0, 64, 1, 3, 0, 64, 1, 3, 0, 64, 1, 7, 224, 64, 1, 7, 224, 64, 1, 7, 192, 64, 1, 7, 192, 64, 1, 1, 128, 64, 1, 1, 128, 64, 1, 1, 0, 64, 1, 1, 0, 64, 1, 1, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 255, 255, 192, 1, 255, 255, 192, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 7, 240, 0, 0, 15, 240, 0, 1, 252, 31, 192, 1, 252, 31, 192, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 128, 64, 1, 0, 128, 64, 1, 1, 128, 64, 1, 1, 128, 64, 1, 3, 128, 64, 1, 3, 224, 64, 1, 7, 224, 64, 1, 7, 224, 64, 1, 7, 192, 64, 1, 0, 192, 64, 1, 1, 128, 64, 1, 1, 128, 64, 1, 1, 0, 64, 1, 1, 0, 64, 1, 1, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 255, 255, 192, 1, 255, 255, 192, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 7, 240, 0, 0, 15, 240, 0, 1, 252, 31, 192, 1, 252, 31, 192, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 128, 64, 1, 0, 128, 64, 1, 1, 128, 64, 1, 1, 128, 64, 1, 1, 128, 64, 1, 3, 240, 64, 1, 3, 224, 64, 1, 7, 224, 64, 1, 7, 224, 64, 1, 0, 192, 64, 1, 0, 192, 64, 1, 0, 128, 64, 1, 0, 128, 64, 1, 1, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 255, 255, 192, 1, 255, 255, 192, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 7, 240, 0, 0, 15, 240, 0, 1, 252, 31, 192, 1, 252, 31, 192, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 128, 64, 1, 0, 128, 64, 1, 1, 128, 64, 1, 1, 128, 64, 1, 1, 128, 64, 1, 3, 240, 64, 1, 3, 224, 64, 1, 7, 224, 64, 1, 7, 224, 64, 1, 0, 192, 64, 1, 0, 192, 64, 1, 0, 128, 64, 1, 0, 128, 64, 1, 1, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 255, 255, 192, 1, 255, 255, 192, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 7, 240, 0, 0, 15, 240, 0, 1, 252, 31, 192, 1, 252, 31, 192, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 128, 64, 1, 0, 128, 64, 1, 1, 128, 64, 1, 1, 128, 64, 1, 1, 128, 64, 1, 3, 240, 64, 1, 3, 224, 64, 1, 7, 224, 64, 1, 7, 224, 64, 1, 0, 192, 64, 1, 0, 192, 64, 1, 0, 128, 64, 1, 0, 128, 64, 1, 1, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 255, 255, 192, 1, 255, 255, 192, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 7, 240, 0, 0, 15, 240, 0, 1, 252, 31, 192, 1, 252, 31, 192, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 128, 64, 1, 0, 128, 64, 1, 1, 128, 64, 1, 1, 128, 64, 1, 1, 128, 64, 1, 3, 240, 64, 1, 3, 224, 64, 1, 7, 224, 64, 1, 7, 224, 64, 1, 0, 192, 64, 1, 0, 192, 64, 1, 0, 128, 64, 1, 0, 128, 64, 1, 1, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 255, 255, 192, 1, 255, 255, 192, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 7, 240, 0, 0, 15, 240, 0, 1, 252, 31, 192, 1, 252, 31, 192, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 128, 64, 1, 0, 128, 64, 1, 1, 128, 64, 1, 1, 128, 64, 1, 1, 128, 64, 1, 3, 240, 64, 1, 3, 224, 64, 1, 7, 224, 64, 1, 7, 224, 64, 1, 0, 192, 64, 1, 0, 192, 64, 1, 0, 128, 64, 1, 0, 128, 64, 1, 1, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 255, 255, 192, 1, 255, 255, 192, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 7, 240, 0, 0, 15, 240, 0, 1, 252, 31, 192, 1, 252, 31, 192, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 128, 64, 1, 0, 128, 64, 1, 1, 128, 64, 1, 1, 128, 64, 1, 1, 128, 64, 1, 3, 240, 64, 1, 3, 224, 64, 1, 7, 224, 64, 1, 7, 224, 64, 1, 0, 192, 64, 1, 0, 192, 64, 1, 0, 128, 64, 1, 0, 128, 64, 1, 1, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 255, 255, 192, 1, 255, 255, 192, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 7, 240, 0, 0, 15, 240, 0, 1, 252, 31, 192, 1, 252, 31, 192, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 128, 64, 1, 0, 128, 64, 1, 1, 128, 64, 1, 1, 128, 64, 1, 1, 128, 64, 1, 3, 240, 64, 1, 3, 224, 64, 1, 7, 224, 64, 1, 7, 224, 64, 1, 0, 192, 64, 1, 0, 192, 64, 1, 0, 128, 64, 1, 0, 128, 64, 1, 1, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 255, 255, 192, 1, 255, 255, 192, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 7, 240, 0, 0, 15, 240, 0, 1, 252, 31, 192, 1, 252, 31, 192, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 128, 64, 1, 0, 128, 64, 1, 1, 128, 64, 1, 1, 128, 64, 1, 1, 128, 64, 1, 3, 240, 64, 1, 3, 224, 64, 1, 7, 224, 64, 1, 7, 224, 64, 1, 0, 192, 64, 1, 0, 192, 64, 1, 0, 128, 64, 1, 0, 128, 64, 1, 1, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 255, 255, 192, 1, 255, 255, 192, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 7, 240, 0, 0, 15, 240, 0, 1, 252, 31, 192, 1, 252, 31, 192, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 128, 64, 1, 0, 128, 64, 1, 0, 128, 64, 1, 1, 128, 64, 1, 1, 240, 64, 1, 1, 240, 64, 1, 3, 240, 64, 1, 3, 224, 64, 1, 0, 96, 64, 1, 0, 96, 64, 1, 0, 64, 64, 1, 0, 64, 64, 1, 0, 64, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 255, 255, 192, 1, 255, 255, 192, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 7, 240, 0, 0, 15, 240, 0, 1, 252, 31, 192, 1, 252, 31, 192, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 64, 64, 1, 0, 64, 64, 1, 0, 64, 64, 1, 0, 192, 64, 1, 0, 192, 64, 1, 0, 192, 64, 1, 0, 248, 64, 1, 0, 248, 64, 1, 1, 248, 64, 1, 1, 240, 64, 1, 0, 48, 64, 1, 0, 48, 64, 1, 0, 48, 64, 1, 0, 48, 64, 1, 0, 32, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 255, 255, 192, 1, 255, 255, 192, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 7, 240, 0, 0, 15, 240, 0, 1, 252, 31, 192, 1, 252, 31, 192, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 64, 64, 1, 0, 64, 64, 1, 0, 64, 64, 1, 0, 64, 64, 1, 0, 192, 64, 1, 0, 224, 64, 1, 0, 248, 64, 1, 0, 248, 64, 1, 0, 248, 64, 1, 0, 248, 64, 1, 0, 24, 64, 1, 0, 24, 64, 1, 0, 24, 64, 1, 0, 16, 64, 1, 0, 16, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 255, 255, 192, 1, 255, 255, 192, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 7, 240, 0, 0, 15, 240, 0, 1, 252, 31, 192, 1, 252, 31, 192, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 64, 64, 1, 0, 64, 64, 1, 0, 192, 64, 1, 0, 192, 64, 1, 0, 192, 64, 1, 0, 200, 64, 1, 1, 248, 64, 1, 1, 240, 64, 1, 1, 240, 64, 1, 1, 240, 64, 1, 0, 48, 64, 1, 0, 32, 64, 1, 0, 32, 64, 1, 0, 32, 64, 1, 0, 32, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 255, 255, 192, 1, 255, 255, 192, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 7, 240, 0, 0, 15, 240, 0, 1, 252, 31, 192, 1, 252, 31, 192, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 128, 64, 1, 0, 128, 64, 1, 1, 128, 64, 1, 1, 128, 64, 1, 3, 224, 64, 1, 3, 224, 64, 1, 7, 192, 64, 1, 1, 192, 64, 1, 0, 128, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 255, 255, 192, 1, 255, 255, 192, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 7, 240, 0, 0, 15, 240, 0, 1, 252, 31, 192, 1, 252, 31, 192, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 128, 64, 1, 1, 128, 64, 1, 3, 0, 64, 1, 3, 128, 64, 1, 6, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 255, 255, 192, 1, 255, 255, 192, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 7, 240, 0, 0, 15, 240, 0, 1, 252, 31, 192, 1, 252, 31, 192, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 255, 255, 192, 1, 255, 255, 192, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 7, 240, 0, 0, 15, 240, 0, 1, 252, 31, 192, 1, 252, 31, 192, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 255, 255, 192, 1, 255, 255, 192, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 7, 240, 0, 0, 15, 240, 0, 1, 252, 31, 192, 1, 252, 31, 192, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 0, 0, 64, 1, 255, 255, 192, 1, 255, 255, 192, 0, 0, 0, 0, 0, 0, 0, 0 }
  };
  int y_mid = 10;
  if (pos == 0) {
    display.fillRoundRect(UI_MIDDLE_X - 2 - 42, UI_MIDDLE_Y - 2, 36, 36, 10, 0);
    display.drawBitmap(UI_MIDDLE_X - 42, UI_MIDDLE_Y, frames[0], FRAME_WIDTH, FRAME_HEIGHT, 1);
  }
  if (pos == 1) {
    display.fillRoundRect(UI_MIDDLE_X - 2, UI_MIDDLE_Y - 2, 36, 36, 10, 0);
    display.drawBitmap(UI_MIDDLE_X, UI_MIDDLE_Y, frames[0], FRAME_WIDTH, FRAME_HEIGHT, 1);
    display.display();
    for (int frame = 0; frame < 28; frame++) {
      if (frame < 14 && !(frame % 7)) {
        // clears previous display
        display.fillRoundRect(UI_MIDDLE_X - 4, y_mid - 4, 40, 40, 10, 0);
        // change display pos for next movement
        y_mid++;
        // fills in with new display
        display.fillRoundRect(UI_MIDDLE_X - 4, y_mid - 4, 40, 40, 10, 1);
        display.fillRoundRect(UI_MIDDLE_X - 2, y_mid - 2, 36, 36, 10, 0);
        // fills in icon onto display
        display.drawBitmap(UI_MIDDLE_X, y_mid, frames[frame], FRAME_WIDTH, FRAME_HEIGHT, 1);
        display.display();
      }
      if (frame >= 14 && !(frame % 7)) {
        display.fillRoundRect(UI_MIDDLE_X - 4, y_mid - 4, 40, 40, 10, 0);
        y_mid--;
        display.fillRoundRect(UI_MIDDLE_X - 4, y_mid - 4, 40, 40, 10, 1);
        display.fillRoundRect(UI_MIDDLE_X - 2, y_mid - 2, 36, 36, 10, 0);
        display.drawBitmap(UI_MIDDLE_X, y_mid, frames[frame], FRAME_WIDTH, FRAME_HEIGHT, 1);
        display.display();
      } else {
        display.fillRoundRect(UI_MIDDLE_X - 4, y_mid - 4, 40, 40, 10, 0);
        display.fillRoundRect(UI_MIDDLE_X - 4, y_mid - 4, 40, 40, 10, 1);
        display.fillRoundRect(UI_MIDDLE_X - 2, y_mid - 2, 36, 36, 10, 0);
        display.drawBitmap(UI_MIDDLE_X, y_mid, frames[frame], FRAME_WIDTH, FRAME_HEIGHT, 1);
        display.display();
      }
    }
  }
  if (pos == 2) {
    display.fillRoundRect(UI_MIDDLE_X - 2 + 42, UI_MIDDLE_Y - 2, 36, 36, 10, 0);
    display.drawBitmap(UI_MIDDLE_X + 42, UI_MIDDLE_Y, frames[0], FRAME_WIDTH, FRAME_HEIGHT, 1);
  }
}

void UI_sandbox(int pos) {
  const byte PROGMEM frames[][128] = {
    { 60, 0, 30, 0, 110, 0, 127, 0, 194, 0, 99, 0, 195, 0, 65, 0, 131, 0, 65, 0, 198, 0, 99, 0, 127, 0, 62, 0, 59, 128, 56, 0, 1, 192, 48, 0, 0, 224, 96, 0, 0, 119, 240, 0, 0, 60, 120, 0, 0, 56, 24, 0, 0, 48, 12, 0, 0, 48, 4, 0, 0, 32, 4, 62, 0, 32, 4, 102, 0, 32, 15, 195, 0, 48, 12, 193, 0, 56, 28, 67, 0, 126, 56, 103, 0, 207, 240, 62, 125, 128, 16, 0, 231, 0, 24, 0, 194, 0, 11, 0, 195, 0, 15, 128, 195, 0, 24, 192, 102, 0, 16, 64, 124, 0, 16, 64, 16, 0, 16, 192, 0, 0, 29, 192, 0, 0, 15, 0 },
    { 60, 0, 30, 0, 110, 0, 127, 0, 194, 0, 99, 0, 195, 0, 65, 0, 131, 0, 65, 0, 198, 0, 99, 0, 127, 0, 62, 0, 59, 128, 56, 0, 1, 192, 48, 0, 0, 224, 96, 0, 0, 119, 240, 0, 0, 60, 120, 0, 0, 56, 24, 0, 0, 48, 12, 0, 0, 48, 4, 0, 0, 32, 4, 62, 0, 32, 4, 102, 0, 32, 15, 195, 0, 48, 12, 193, 0, 56, 28, 67, 0, 126, 56, 103, 0, 207, 240, 62, 125, 128, 16, 0, 231, 0, 24, 0, 194, 0, 11, 0, 195, 0, 15, 128, 195, 0, 24, 192, 102, 0, 16, 64, 124, 0, 16, 64, 16, 0, 16, 192, 0, 0, 29, 192, 0, 0, 15, 0 },
    { 60, 0, 30, 0, 110, 0, 127, 0, 194, 0, 99, 0, 195, 0, 65, 0, 131, 0, 65, 0, 198, 0, 99, 0, 127, 0, 62, 0, 59, 128, 56, 0, 1, 192, 48, 0, 0, 224, 96, 0, 0, 119, 240, 0, 0, 60, 120, 0, 0, 56, 24, 0, 0, 48, 12, 0, 0, 48, 4, 0, 0, 32, 4, 62, 0, 32, 4, 102, 0, 32, 15, 195, 0, 48, 12, 193, 0, 56, 28, 67, 0, 126, 56, 103, 0, 207, 240, 62, 125, 128, 16, 0, 231, 0, 24, 0, 194, 0, 11, 0, 195, 0, 15, 128, 195, 0, 24, 192, 102, 0, 16, 64, 124, 0, 16, 64, 16, 0, 16, 192, 0, 0, 29, 192, 0, 0, 15, 0 },
    { 60, 0, 30, 0, 110, 0, 127, 0, 194, 0, 99, 0, 195, 0, 65, 0, 131, 0, 65, 0, 198, 0, 99, 0, 127, 0, 62, 0, 59, 128, 56, 0, 1, 192, 48, 0, 0, 224, 96, 0, 0, 119, 240, 0, 0, 60, 120, 0, 0, 56, 24, 0, 0, 48, 12, 0, 0, 48, 4, 0, 0, 32, 4, 62, 0, 32, 4, 102, 0, 32, 15, 195, 0, 48, 12, 193, 0, 56, 28, 67, 0, 126, 56, 103, 0, 207, 240, 62, 125, 128, 16, 0, 231, 0, 24, 0, 194, 0, 11, 0, 195, 0, 15, 128, 195, 0, 24, 192, 102, 0, 16, 64, 124, 0, 16, 64, 16, 0, 16, 192, 0, 0, 29, 192, 0, 0, 15, 0 },
    { 60, 0, 30, 0, 110, 0, 63, 0, 194, 0, 99, 0, 195, 0, 65, 0, 195, 0, 65, 128, 67, 0, 99, 0, 127, 0, 63, 0, 59, 128, 60, 0, 1, 192, 48, 0, 0, 224, 32, 0, 0, 119, 240, 0, 0, 60, 120, 0, 0, 56, 24, 0, 0, 48, 12, 0, 0, 48, 4, 0, 0, 32, 4, 62, 0, 32, 4, 102, 0, 32, 15, 195, 0, 48, 12, 195, 0, 56, 28, 195, 0, 126, 56, 103, 0, 207, 240, 62, 127, 128, 16, 16, 231, 0, 24, 0, 194, 0, 15, 0, 131, 0, 31, 128, 195, 0, 24, 192, 102, 0, 48, 64, 124, 0, 16, 64, 0, 0, 16, 192, 0, 0, 29, 128, 0, 0, 15, 0 },
    { 60, 0, 12, 0, 126, 0, 63, 0, 67, 0, 99, 0, 193, 0, 97, 128, 67, 0, 65, 128, 99, 0, 97, 128, 127, 0, 59, 0, 25, 128, 30, 0, 0, 192, 48, 0, 0, 96, 48, 0, 0, 55, 240, 0, 0, 28, 120, 0, 0, 56, 24, 0, 0, 48, 12, 0, 0, 48, 4, 0, 0, 32, 4, 60, 0, 32, 4, 110, 0, 32, 15, 195, 0, 48, 13, 195, 0, 56, 28, 195, 0, 126, 56, 67, 29, 207, 224, 126, 127, 128, 48, 24, 198, 0, 16, 0, 195, 0, 31, 0, 131, 0, 31, 128, 195, 0, 48, 128, 126, 0, 48, 192, 56, 0, 48, 192, 0, 0, 48, 192, 0, 0, 31, 128, 0, 0, 15, 0 },
    { 30, 0, 0, 0, 55, 0, 31, 0, 97, 0, 51, 128, 65, 0, 32, 128, 65, 128, 96, 128, 99, 0, 33, 128, 63, 0, 49, 128, 25, 128, 31, 0, 0, 192, 48, 0, 0, 96, 48, 0, 0, 55, 240, 0, 0, 28, 120, 0, 0, 56, 24, 0, 0, 48, 12, 0, 0, 48, 4, 0, 0, 32, 4, 28, 0, 32, 4, 126, 0, 32, 15, 194, 0, 48, 15, 195, 0, 56, 28, 131, 0, 254, 56, 195, 125, 143, 224, 126, 231, 0, 48, 56, 194, 0, 16, 0, 195, 0, 30, 0, 195, 0, 63, 0, 102, 0, 49, 128, 124, 0, 32, 128, 0, 0, 32, 128, 0, 0, 49, 128, 0, 0, 63, 0, 0, 0, 30, 0 },
    { 14, 0, 0, 0, 63, 0, 7, 0, 33, 128, 31, 128, 96, 128, 48, 128, 33, 128, 48, 192, 49, 128, 32, 192, 63, 128, 48, 192, 13, 192, 31, 128, 0, 192, 30, 0, 0, 96, 48, 0, 0, 55, 240, 0, 0, 28, 120, 0, 0, 56, 24, 0, 0, 48, 12, 0, 0, 48, 4, 0, 0, 32, 4, 0, 0, 32, 4, 124, 0, 32, 15, 198, 0, 48, 15, 194, 0, 120, 28, 131, 60, 222, 56, 194, 111, 143, 224, 230, 194, 0, 32, 124, 195, 0, 48, 0, 195, 0, 30, 0, 70, 0, 63, 0, 126, 0, 97, 0, 16, 0, 97, 128, 0, 0, 65, 128, 0, 0, 99, 0, 0, 0, 63, 0, 0, 0, 28, 0 },
    { 15, 0, 0, 0, 27, 128, 0, 0, 48, 192, 15, 128, 48, 192, 25, 192, 48, 192, 24, 64, 16, 192, 16, 64, 31, 128, 16, 64, 6, 192, 24, 192, 0, 96, 15, 128, 0, 48, 24, 0, 0, 63, 240, 0, 0, 28, 120, 0, 0, 56, 24, 0, 0, 48, 12, 0, 0, 48, 4, 0, 0, 32, 4, 0, 0, 32, 4, 60, 0, 32, 14, 254, 0, 48, 15, 198, 60, 248, 29, 130, 111, 158, 56, 130, 195, 15, 224, 198, 195, 0, 32, 124, 195, 0, 32, 48, 198, 0, 60, 0, 126, 0, 110, 0, 16, 0, 194, 0, 0, 0, 195, 0, 0, 0, 195, 0, 0, 0, 67, 0, 0, 0, 126, 0, 0, 0, 24, 0 },
    { 7, 128, 0, 0, 13, 192, 0, 0, 24, 64, 3, 128, 24, 96, 15, 192, 24, 96, 28, 64, 8, 192, 24, 96, 15, 192, 24, 96, 7, 96, 8, 96, 0, 96, 14, 192, 0, 48, 15, 128, 0, 31, 248, 0, 0, 28, 120, 0, 0, 56, 24, 0, 0, 48, 12, 0, 0, 48, 4, 0, 0, 32, 4, 0, 0, 32, 4, 16, 0, 32, 12, 252, 124, 112, 15, 196, 231, 216, 29, 134, 195, 30, 57, 134, 195, 15, 224, 134, 195, 0, 64, 236, 102, 0, 96, 120, 124, 0, 120, 0, 16, 0, 204, 0, 0, 1, 134, 0, 0, 1, 134, 0, 0, 1, 134, 0, 0, 0, 206, 0, 0, 0, 252, 0, 0, 0, 32, 0 },
    { 3, 192, 0, 0, 6, 224, 0, 0, 12, 32, 0, 0, 12, 48, 3, 192, 8, 48, 6, 224, 12, 96, 12, 32, 7, 224, 12, 48, 3, 224, 8, 48, 0, 48, 12, 96, 0, 24, 15, 224, 0, 31, 255, 128, 0, 28, 120, 0, 0, 56, 24, 0, 0, 48, 12, 0, 0, 48, 4, 0, 0, 32, 4, 0, 0, 32, 4, 0, 124, 32, 12, 120, 199, 240, 15, 220, 195, 24, 31, 132, 131, 30, 57, 134, 195, 15, 225, 6, 102, 0, 193, 140, 124, 0, 64, 252, 0, 1, 240, 112, 0, 3, 152, 0, 0, 3, 12, 0, 0, 3, 12, 0, 0, 3, 12, 0, 0, 1, 152, 0, 0, 1, 248, 0, 0, 0, 0, 0 },
    { 1, 224, 0, 0, 3, 112, 0, 0, 6, 16, 0, 0, 6, 24, 0, 0, 4, 24, 3, 224, 6, 48, 6, 112, 3, 240, 6, 16, 1, 240, 4, 16, 0, 16, 4, 16, 0, 24, 6, 48, 0, 15, 255, 224, 0, 28, 124, 0, 0, 56, 24, 0, 0, 48, 12, 0, 0, 48, 4, 0, 28, 32, 4, 0, 126, 32, 4, 0, 195, 224, 12, 0, 195, 176, 14, 248, 131, 24, 31, 156, 195, 30, 57, 4, 126, 15, 225, 4, 56, 0, 129, 12, 0, 0, 129, 140, 0, 3, 224, 248, 0, 6, 48, 0, 0, 6, 16, 0, 0, 4, 24, 0, 0, 6, 24, 0, 0, 3, 48, 0, 0, 3, 224, 0, 0, 0, 0, 0 },
    { 0, 240, 0, 0, 1, 184, 0, 0, 3, 8, 0, 0, 3, 12, 0, 0, 3, 12, 0, 64, 1, 12, 3, 240, 1, 248, 3, 16, 0, 248, 6, 24, 0, 24, 6, 24, 0, 12, 2, 24, 0, 15, 247, 176, 0, 28, 127, 224, 0, 56, 24, 0, 0, 48, 12, 0, 124, 48, 4, 0, 230, 32, 4, 0, 195, 224, 4, 0, 195, 224, 12, 0, 195, 48, 12, 112, 70, 24, 31, 248, 124, 30, 59, 8, 16, 15, 227, 12, 0, 1, 131, 12, 0, 1, 129, 12, 0, 7, 193, 248, 0, 12, 96, 240, 0, 8, 32, 0, 0, 8, 48, 0, 0, 12, 96, 0, 0, 14, 96, 0, 0, 7, 192, 0, 0, 0, 0, 0 },
    { 0, 120, 0, 0, 0, 220, 0, 0, 1, 134, 0, 0, 1, 134, 0, 0, 1, 134, 0, 0, 0, 134, 0, 96, 0, 252, 1, 248, 0, 56, 3, 24, 0, 12, 3, 8, 0, 4, 2, 12, 0, 7, 243, 8, 0, 28, 127, 248, 24, 56, 28, 224, 126, 48, 12, 0, 198, 48, 4, 0, 195, 224, 4, 0, 131, 224, 4, 0, 195, 32, 12, 0, 118, 48, 12, 0, 60, 24, 31, 240, 0, 30, 63, 184, 0, 15, 227, 8, 0, 3, 2, 8, 0, 7, 2, 12, 0, 31, 131, 24, 0, 24, 193, 240, 0, 48, 64, 64, 0, 16, 64, 0, 0, 24, 192, 0, 0, 31, 128, 0, 0, 15, 0, 0, 0, 0, 0, 0 },
    { 0, 60, 0, 0, 0, 110, 0, 0, 0, 195, 0, 0, 0, 193, 0, 0, 0, 195, 0, 0, 0, 99, 0, 0, 0, 126, 0, 112, 0, 28, 1, 248, 0, 4, 1, 12, 0, 6, 3, 4, 0, 7, 241, 12, 60, 28, 123, 140, 110, 56, 31, 248, 194, 48, 12, 224, 195, 176, 4, 0, 195, 224, 4, 0, 70, 32, 4, 0, 126, 32, 12, 0, 16, 48, 12, 0, 0, 24, 28, 64, 0, 30, 63, 240, 0, 15, 227, 24, 0, 6, 6, 8, 0, 30, 6, 8, 0, 63, 2, 24, 0, 97, 131, 184, 0, 96, 129, 240, 0, 97, 128, 0, 0, 33, 128, 0, 0, 63, 0, 0, 0, 28, 0, 0, 0, 0, 0, 0 },
    { 0, 14, 0, 0, 0, 63, 0, 0, 0, 33, 128, 0, 0, 96, 128, 0, 0, 33, 128, 0, 0, 49, 128, 0, 0, 63, 0, 0, 0, 14, 0, 120, 0, 2, 1, 220, 24, 2, 1, 132, 126, 7, 241, 4, 198, 28, 121, 4, 195, 56, 27, 140, 131, 176, 14, 248, 195, 240, 4, 96, 118, 32, 4, 0, 60, 32, 4, 0, 0, 32, 12, 0, 0, 48, 12, 0, 0, 24, 28, 0, 0, 30, 60, 224, 0, 15, 231, 240, 0, 4, 6, 16, 0, 60, 6, 24, 0, 110, 4, 24, 0, 194, 6, 24, 0, 195, 3, 240, 0, 195, 1, 192, 0, 70, 0, 0, 0, 126, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0 },
    { 0, 7, 128, 0, 0, 31, 192, 0, 0, 24, 192, 0, 0, 48, 64, 0, 0, 16, 64, 0, 0, 24, 192, 0, 0, 15, 128, 0, 0, 7, 0, 0, 60, 3, 0, 120, 110, 3, 0, 220, 194, 7, 241, 132, 195, 28, 121, 134, 131, 184, 25, 134, 199, 240, 15, 134, 126, 112, 4, 252, 16, 32, 4, 112, 0, 32, 4, 0, 0, 32, 12, 0, 0, 48, 12, 0, 0, 24, 28, 0, 0, 30, 60, 0, 0, 15, 239, 224, 0, 12, 6, 112, 0, 248, 6, 16, 1, 140, 4, 16, 1, 132, 4, 16, 1, 4, 6, 48, 1, 4, 3, 224, 1, 204, 0, 128, 0, 248, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 3, 192, 0, 0, 15, 224, 0, 0, 12, 96, 0, 0, 8, 32, 0, 0, 8, 32, 0, 0, 12, 96, 0, 8, 7, 192, 0, 124, 3, 0, 0, 198, 1, 0, 0, 194, 1, 0, 56, 131, 7, 240, 252, 195, 28, 120, 134, 103, 248, 25, 130, 124, 240, 13, 134, 0, 48, 7, 198, 0, 32, 4, 252, 0, 32, 4, 48, 0, 32, 12, 0, 0, 48, 12, 0, 0, 24, 28, 0, 0, 30, 56, 0, 0, 31, 236, 128, 0, 88, 7, 224, 1, 240, 6, 32, 3, 24, 12, 48, 2, 8, 12, 48, 2, 8, 4, 48, 2, 24, 7, 96, 3, 152, 3, 192, 1, 240, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 1, 224, 0, 0, 7, 240, 0, 0, 6, 48, 0, 0, 4, 16, 0, 0, 4, 16, 0, 28, 6, 48, 0, 126, 3, 224, 0, 194, 1, 128, 0, 195, 0, 128, 0, 131, 1, 128, 0, 195, 7, 240, 60, 127, 220, 120, 254, 56, 248, 24, 198, 0, 48, 13, 130, 0, 48, 7, 130, 0, 32, 6, 198, 0, 32, 4, 124, 0, 32, 12, 48, 0, 48, 12, 0, 0, 24, 28, 0, 0, 30, 56, 0, 0, 31, 248, 0, 0, 240, 15, 192, 7, 224, 6, 224, 6, 48, 12, 32, 12, 16, 12, 48, 12, 16, 8, 48, 4, 48, 12, 96, 7, 96, 7, 224, 3, 192, 3, 128, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 240, 0, 0, 3, 248, 0, 0, 3, 24, 0, 0, 6, 8, 0, 60, 2, 8, 0, 110, 3, 24, 0, 194, 1, 240, 0, 195, 0, 192, 0, 131, 0, 192, 0, 199, 0, 192, 0, 127, 135, 240, 0, 48, 252, 120, 60, 0, 120, 24, 126, 0, 48, 12, 198, 0, 48, 4, 130, 0, 32, 7, 131, 0, 32, 4, 198, 0, 32, 12, 126, 0, 48, 12, 56, 0, 24, 28, 0, 0, 30, 56, 0, 0, 63, 248, 0, 3, 224, 12, 0, 15, 224, 15, 192, 12, 96, 12, 96, 24, 32, 8, 32, 24, 96, 8, 32, 8, 96, 8, 96, 15, 192, 14, 96, 7, 128, 7, 192, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 112, 0, 0, 1, 248, 0, 0, 1, 12, 0, 60, 3, 4, 0, 110, 1, 12, 0, 194, 1, 140, 0, 195, 1, 248, 0, 195, 0, 96, 0, 71, 0, 64, 0, 127, 128, 192, 0, 17, 199, 240, 0, 0, 124, 120, 0, 0, 56, 24, 60, 0, 48, 12, 110, 0, 48, 4, 194, 0, 32, 7, 195, 0, 32, 7, 131, 0, 32, 12, 198, 0, 48, 12, 126, 0, 24, 28, 48, 0, 62, 56, 0, 0, 127, 248, 0, 7, 224, 24, 0, 31, 192, 15, 128, 16, 192, 15, 192, 48, 64, 24, 96, 16, 192, 24, 32, 24, 192, 24, 96, 31, 128, 12, 96, 14, 0, 15, 192, 0, 0, 7, 0, 0, 0, 0, 0 },
    { 0, 0, 120, 0, 0, 0, 220, 0, 60, 1, 134, 0, 110, 1, 134, 0, 194, 1, 134, 0, 195, 0, 134, 0, 195, 0, 252, 0, 198, 0, 112, 0, 127, 0, 96, 0, 17, 192, 64, 0, 0, 231, 240, 0, 0, 124, 120, 0, 0, 56, 24, 0, 0, 48, 12, 60, 0, 48, 4, 102, 0, 32, 4, 194, 0, 32, 7, 195, 0, 32, 12, 195, 0, 48, 12, 102, 0, 24, 28, 126, 0, 62, 56, 16, 0, 111, 240, 0, 15, 192, 24, 0, 59, 128, 9, 0, 49, 128, 15, 192, 32, 128, 24, 192, 32, 128, 24, 96, 49, 128, 24, 96, 31, 0, 24, 96, 12, 0, 12, 192, 0, 0, 7, 128, 0, 0, 0, 0 },
    { 0, 0, 60, 0, 60, 0, 238, 0, 126, 0, 194, 0, 194, 0, 130, 0, 195, 0, 131, 0, 131, 0, 198, 0, 194, 0, 124, 0, 127, 0, 48, 0, 57, 128, 96, 0, 0, 192, 96, 0, 0, 103, 240, 0, 0, 60, 120, 0, 0, 56, 24, 0, 0, 48, 12, 8, 0, 48, 4, 126, 0, 32, 4, 230, 0, 32, 7, 195, 0, 32, 15, 195, 0, 48, 12, 195, 0, 24, 28, 102, 0, 62, 56, 60, 0, 239, 240, 0, 31, 192, 24, 0, 55, 128, 24, 0, 97, 0, 15, 128, 97, 128, 29, 192, 65, 128, 24, 64, 99, 0, 16, 96, 63, 0, 16, 96, 8, 0, 24, 192, 0, 0, 15, 128, 0, 0, 2, 0 },
    { 8, 0, 60, 0, 124, 0, 110, 0, 198, 0, 195, 0, 194, 0, 193, 0, 131, 0, 195, 0, 195, 0, 67, 0, 102, 0, 126, 0, 127, 0, 56, 0, 1, 128, 32, 0, 0, 192, 96, 0, 0, 103, 240, 0, 0, 60, 120, 0, 0, 56, 24, 0, 0, 48, 12, 0, 0, 48, 4, 28, 0, 32, 4, 126, 0, 32, 7, 195, 0, 32, 15, 195, 0, 48, 12, 195, 0, 24, 28, 67, 0, 126, 56, 126, 0, 239, 240, 56, 63, 128, 16, 0, 103, 0, 24, 0, 67, 0, 15, 128, 193, 0, 31, 192, 67, 0, 24, 192, 99, 0, 16, 64, 62, 0, 16, 64, 16, 0, 24, 192, 0, 0, 15, 128, 0, 0, 7, 0 },
    { 60, 0, 30, 0, 126, 0, 127, 0, 194, 0, 99, 0, 195, 0, 193, 0, 131, 0, 65, 0, 194, 0, 99, 0, 127, 0, 62, 0, 59, 128, 56, 0, 1, 192, 48, 0, 0, 224, 96, 0, 0, 119, 240, 0, 0, 60, 120, 0, 0, 56, 24, 0, 0, 48, 12, 0, 0, 48, 4, 8, 0, 32, 4, 62, 0, 32, 6, 102, 0, 32, 15, 195, 0, 48, 12, 195, 0, 56, 28, 67, 0, 126, 56, 103, 0, 207, 240, 62, 61, 128, 16, 0, 103, 0, 24, 0, 195, 0, 15, 0, 195, 0, 15, 128, 195, 0, 24, 192, 103, 0, 16, 64, 126, 0, 16, 64, 16, 0, 24, 192, 0, 0, 31, 192, 0, 0, 15, 0 },
    { 60, 0, 30, 0, 110, 0, 127, 0, 194, 0, 99, 0, 195, 0, 65, 0, 131, 0, 65, 0, 198, 0, 99, 0, 127, 0, 62, 0, 59, 128, 56, 0, 1, 192, 48, 0, 0, 224, 96, 0, 0, 119, 240, 0, 0, 60, 120, 0, 0, 56, 24, 0, 0, 48, 12, 0, 0, 48, 4, 0, 0, 32, 4, 62, 0, 32, 4, 102, 0, 32, 15, 195, 0, 48, 12, 193, 0, 56, 28, 67, 0, 126, 56, 103, 0, 207, 240, 62, 125, 128, 16, 0, 231, 0, 24, 0, 194, 0, 11, 0, 195, 0, 15, 128, 195, 0, 24, 192, 102, 0, 16, 64, 124, 0, 16, 64, 16, 0, 16, 192, 0, 0, 29, 192, 0, 0, 15, 0 },
    { 60, 0, 30, 0, 110, 0, 127, 0, 194, 0, 99, 0, 195, 0, 65, 0, 131, 0, 65, 0, 198, 0, 99, 0, 127, 0, 62, 0, 59, 128, 56, 0, 1, 192, 48, 0, 0, 224, 96, 0, 0, 119, 240, 0, 0, 60, 120, 0, 0, 56, 24, 0, 0, 48, 12, 0, 0, 48, 4, 0, 0, 32, 4, 62, 0, 32, 4, 102, 0, 32, 15, 195, 0, 48, 12, 193, 0, 56, 28, 67, 0, 126, 56, 103, 0, 207, 240, 62, 125, 128, 16, 0, 231, 0, 24, 0, 194, 0, 11, 0, 195, 0, 15, 128, 195, 0, 24, 192, 102, 0, 16, 64, 124, 0, 16, 64, 16, 0, 16, 192, 0, 0, 29, 192, 0, 0, 15, 0 },
    { 60, 0, 30, 0, 110, 0, 127, 0, 194, 0, 99, 0, 195, 0, 65, 0, 131, 0, 65, 0, 198, 0, 99, 0, 127, 0, 62, 0, 59, 128, 56, 0, 1, 192, 48, 0, 0, 224, 96, 0, 0, 119, 240, 0, 0, 60, 120, 0, 0, 56, 24, 0, 0, 48, 12, 0, 0, 48, 4, 0, 0, 32, 4, 62, 0, 32, 4, 102, 0, 32, 15, 195, 0, 48, 12, 193, 0, 56, 28, 67, 0, 126, 56, 103, 0, 207, 240, 62, 125, 128, 16, 0, 231, 0, 24, 0, 194, 0, 11, 0, 195, 0, 15, 128, 195, 0, 24, 192, 102, 0, 16, 64, 124, 0, 16, 64, 16, 0, 16, 192, 0, 0, 29, 192, 0, 0, 15, 0 }
  };
  int y_mid = 10;
  if (pos == 0) {
    display.fillRoundRect(UI_MIDDLE_X - 2 - 42, UI_MIDDLE_Y - 2, 36, 36, 10, 0);
    display.drawBitmap(UI_MIDDLE_X - 42, UI_MIDDLE_Y, frames[0], FRAME_WIDTH, FRAME_HEIGHT, 1);
  }
  if (pos == 1) {
    display.fillRoundRect(UI_MIDDLE_X - 2, UI_MIDDLE_Y - 2, 36, 36, 10, 0);
    display.drawBitmap(UI_MIDDLE_X, UI_MIDDLE_Y, frames[0], FRAME_WIDTH, FRAME_HEIGHT, 1);
    display.display();
    for (int frame = 0; frame < 28; frame++) {
      if (frame < 14 && !(frame % 7)) {
        // clears previous display
        display.fillRoundRect(UI_MIDDLE_X - 4, y_mid - 4, 40, 40, 10, 0);
        // change display pos for next movement
        y_mid++;
        // fills in with new display
        display.fillRoundRect(UI_MIDDLE_X - 4, y_mid - 4, 40, 40, 10, 1);
        display.fillRoundRect(UI_MIDDLE_X - 2, y_mid - 2, 36, 36, 10, 0);
        // fills in icon onto display
        display.drawBitmap(UI_MIDDLE_X, y_mid, frames[frame], FRAME_WIDTH, FRAME_HEIGHT, 1);
        display.display();
      }
      if (frame >= 14 && !(frame % 7)) {
        display.fillRoundRect(UI_MIDDLE_X - 4, y_mid - 4, 40, 40, 10, 0);
        y_mid--;
        display.fillRoundRect(UI_MIDDLE_X - 4, y_mid - 4, 40, 40, 10, 1);
        display.fillRoundRect(UI_MIDDLE_X - 2, y_mid - 2, 36, 36, 10, 0);
        display.drawBitmap(UI_MIDDLE_X, y_mid, frames[frame], FRAME_WIDTH, FRAME_HEIGHT, 1);
        display.display();
      } else {
        display.fillRoundRect(UI_MIDDLE_X - 4, y_mid - 4, 40, 40, 10, 0);
        display.fillRoundRect(UI_MIDDLE_X - 4, y_mid - 4, 40, 40, 10, 1);
        display.fillRoundRect(UI_MIDDLE_X - 2, y_mid - 2, 36, 36, 10, 0);
        display.drawBitmap(UI_MIDDLE_X, y_mid, frames[frame], FRAME_WIDTH, FRAME_HEIGHT, 1);
        display.display();
      }
    }
  }
  if (pos == 2) {
    display.fillRoundRect(UI_MIDDLE_X - 2 + 42, UI_MIDDLE_Y - 2, 36, 36, 10, 0);
    display.drawBitmap(UI_MIDDLE_X + 42, UI_MIDDLE_Y, frames[0], FRAME_WIDTH, FRAME_HEIGHT, 1);
  }
}

void UI_tinker_corner(int pos) {
  const byte PROGMEM frames[][128] = {
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 240, 0, 0, 28, 120, 0, 0, 112, 14, 0, 0, 96, 6, 0, 0, 192, 3, 0, 1, 128, 1, 0, 1, 128, 1, 128, 1, 128, 1, 128, 1, 128, 0, 128, 1, 128, 1, 128, 1, 128, 1, 128, 1, 128, 1, 128, 0, 128, 3, 0, 0, 192, 3, 0, 0, 96, 6, 0, 0, 48, 12, 0, 0, 48, 28, 0, 0, 24, 24, 0, 0, 15, 240, 0, 0, 15, 240, 0, 0, 12, 48, 0, 0, 13, 240, 0, 0, 13, 240, 0, 0, 7, 224, 0, 0, 1, 128, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 240, 0, 0, 28, 120, 0, 0, 112, 14, 0, 0, 96, 6, 0, 0, 192, 3, 0, 1, 128, 1, 0, 1, 128, 1, 128, 1, 128, 1, 128, 1, 128, 0, 128, 1, 128, 1, 128, 1, 128, 1, 128, 1, 128, 1, 128, 0, 128, 3, 0, 0, 192, 3, 0, 0, 96, 6, 0, 0, 48, 12, 0, 0, 48, 28, 0, 0, 24, 24, 0, 0, 15, 240, 0, 0, 15, 240, 0, 0, 12, 48, 0, 0, 13, 240, 0, 0, 13, 240, 0, 0, 7, 224, 0, 0, 1, 128, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 240, 0, 0, 28, 120, 0, 0, 112, 14, 0, 0, 96, 6, 0, 0, 192, 3, 0, 1, 128, 1, 0, 1, 128, 1, 128, 1, 128, 1, 128, 1, 128, 0, 128, 1, 128, 1, 128, 1, 128, 1, 128, 1, 128, 1, 128, 0, 128, 3, 0, 0, 192, 3, 0, 0, 96, 6, 0, 0, 48, 12, 0, 0, 48, 28, 0, 0, 24, 24, 0, 0, 15, 240, 0, 0, 15, 240, 0, 0, 12, 48, 0, 0, 13, 240, 0, 0, 13, 240, 0, 0, 7, 224, 0, 0, 1, 128, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 240, 0, 0, 28, 124, 0, 0, 48, 14, 0, 0, 96, 2, 0, 0, 192, 3, 0, 0, 192, 1, 0, 0, 128, 1, 128, 1, 128, 0, 128, 1, 128, 0, 128, 1, 128, 0, 128, 1, 128, 1, 128, 0, 128, 1, 128, 0, 192, 1, 128, 0, 64, 3, 0, 0, 96, 7, 0, 0, 48, 14, 0, 0, 24, 12, 0, 0, 24, 24, 0, 0, 15, 240, 0, 0, 15, 240, 0, 0, 12, 48, 0, 0, 13, 240, 0, 0, 13, 240, 0, 0, 7, 240, 0, 0, 1, 128, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 135, 249, 0, 0, 30, 61, 0, 0, 56, 14, 0, 0, 96, 3, 0, 0, 96, 1, 0, 0, 192, 1, 128, 0, 192, 0, 128, 0, 128, 0, 128, 8, 128, 0, 240, 0, 128, 0, 192, 0, 128, 0, 128, 0, 128, 0, 128, 0, 192, 1, 128, 0, 96, 3, 128, 0, 96, 3, 0, 0, 48, 6, 0, 0, 152, 13, 0, 0, 8, 24, 0, 0, 15, 248, 0, 0, 15, 240, 0, 0, 12, 16, 0, 0, 14, 240, 0, 0, 14, 112, 0, 0, 7, 240, 0, 0, 1, 128, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 135, 249, 128, 0, 14, 61, 0, 0, 56, 6, 0, 0, 112, 3, 0, 0, 96, 1, 128, 0, 192, 0, 128, 0, 192, 0, 128, 0, 192, 0, 192, 8, 128, 0, 240, 0, 128, 0, 192, 0, 192, 0, 192, 0, 192, 0, 128, 0, 64, 1, 128, 0, 96, 1, 128, 0, 48, 3, 0, 0, 48, 6, 0, 1, 152, 13, 128, 1, 8, 24, 0, 0, 15, 248, 0, 0, 15, 240, 0, 0, 12, 16, 0, 0, 14, 112, 0, 0, 14, 48, 0, 0, 7, 240, 0, 0, 1, 128, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 0, 0, 0, 1, 0, 0, 128, 1, 135, 241, 128, 0, 28, 61, 0, 0, 48, 14, 0, 0, 96, 3, 0, 0, 192, 1, 0, 0, 192, 1, 128, 0, 128, 0, 128, 1, 128, 0, 128, 25, 128, 0, 184, 0, 128, 0, 128, 0, 128, 0, 128, 0, 128, 1, 128, 0, 192, 1, 128, 0, 64, 3, 0, 0, 96, 3, 0, 0, 48, 6, 0, 1, 152, 13, 128, 1, 8, 24, 128, 0, 15, 240, 0, 0, 15, 240, 0, 0, 12, 16, 0, 0, 15, 176, 0, 0, 15, 176, 0, 0, 7, 240, 0, 0, 1, 128, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 0, 0, 0, 3, 0, 0, 192, 1, 143, 241, 128, 0, 60, 121, 0, 0, 112, 28, 0, 0, 192, 6, 0, 0, 192, 3, 0, 1, 128, 3, 0, 1, 128, 1, 0, 1, 0, 1, 0, 57, 0, 1, 188, 1, 0, 1, 128, 1, 0, 1, 0, 1, 0, 3, 0, 1, 128, 3, 0, 0, 192, 7, 0, 0, 64, 6, 0, 0, 96, 12, 0, 1, 176, 25, 128, 3, 24, 24, 192, 2, 15, 240, 0, 0, 15, 240, 0, 0, 8, 48, 0, 0, 15, 176, 0, 0, 15, 176, 0, 0, 7, 224, 0, 0, 1, 128, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 0, 0, 0, 3, 0, 0, 192, 1, 143, 225, 128, 0, 56, 249, 0, 0, 96, 28, 0, 0, 192, 12, 0, 1, 128, 6, 0, 1, 128, 2, 0, 3, 0, 3, 0, 3, 0, 1, 0, 59, 0, 1, 60, 3, 0, 1, 0, 3, 0, 3, 0, 1, 0, 3, 0, 1, 128, 3, 0, 1, 128, 6, 0, 0, 192, 14, 0, 0, 96, 12, 0, 1, 176, 25, 128, 3, 24, 48, 192, 2, 15, 240, 64, 0, 15, 240, 0, 0, 8, 48, 0, 0, 11, 240, 0, 0, 13, 240, 0, 0, 7, 224, 0, 0, 1, 128, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 0, 0, 0, 3, 0, 0, 192, 1, 143, 225, 128, 0, 56, 121, 0, 0, 96, 28, 0, 0, 192, 6, 0, 1, 128, 6, 0, 1, 128, 3, 0, 1, 0, 1, 0, 3, 0, 1, 0, 59, 0, 1, 60, 3, 0, 1, 0, 1, 0, 1, 0, 1, 0, 3, 0, 1, 128, 3, 0, 0, 128, 6, 0, 0, 192, 6, 0, 0, 96, 12, 0, 1, 176, 25, 128, 3, 24, 16, 192, 2, 15, 240, 64, 0, 15, 240, 0, 0, 8, 48, 0, 0, 9, 240, 0, 0, 13, 240, 0, 0, 7, 224, 0, 0, 1, 128, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 0, 0, 0, 3, 0, 0, 192, 1, 143, 241, 128, 0, 28, 121, 0, 0, 112, 12, 0, 0, 192, 6, 0, 0, 192, 3, 0, 1, 128, 1, 0, 1, 128, 1, 0, 1, 128, 1, 128, 57, 0, 1, 188, 1, 0, 1, 128, 1, 0, 1, 128, 1, 128, 1, 0, 0, 128, 3, 0, 0, 192, 3, 0, 0, 96, 6, 0, 0, 96, 12, 0, 1, 176, 25, 128, 3, 24, 24, 192, 2, 15, 240, 64, 0, 15, 240, 0, 0, 12, 48, 0, 0, 15, 176, 0, 0, 15, 176, 0, 0, 7, 224, 0, 0, 1, 128, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 0, 0, 0, 3, 0, 0, 192, 1, 135, 241, 128, 0, 28, 121, 0, 0, 112, 14, 0, 0, 96, 6, 0, 0, 192, 3, 0, 1, 128, 1, 0, 1, 128, 1, 128, 1, 128, 1, 128, 57, 128, 0, 188, 1, 128, 1, 128, 1, 128, 1, 128, 1, 128, 1, 128, 0, 128, 3, 0, 0, 192, 3, 0, 0, 96, 6, 0, 0, 48, 12, 0, 1, 176, 29, 128, 3, 24, 24, 192, 2, 15, 240, 64, 0, 15, 240, 0, 0, 12, 48, 0, 0, 15, 48, 0, 0, 15, 48, 0, 0, 7, 224, 0, 0, 1, 128, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 0, 0, 0, 3, 0, 0, 192, 1, 135, 241, 128, 0, 28, 121, 0, 0, 112, 14, 0, 0, 96, 6, 0, 0, 192, 3, 0, 1, 128, 1, 0, 1, 128, 1, 128, 1, 128, 1, 128, 57, 128, 0, 188, 1, 128, 1, 128, 1, 128, 1, 128, 1, 128, 1, 128, 0, 128, 3, 0, 0, 192, 3, 0, 0, 96, 6, 0, 0, 48, 12, 0, 1, 176, 29, 128, 3, 24, 24, 192, 2, 15, 240, 64, 0, 15, 240, 0, 0, 12, 48, 0, 0, 15, 48, 0, 0, 15, 48, 0, 0, 7, 224, 0, 0, 1, 128, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 0, 0, 0, 3, 0, 0, 192, 1, 135, 241, 128, 0, 28, 121, 0, 0, 112, 14, 0, 0, 96, 6, 0, 0, 192, 3, 0, 1, 128, 1, 0, 1, 128, 1, 128, 1, 128, 1, 128, 57, 128, 0, 188, 1, 128, 1, 128, 1, 128, 1, 128, 1, 128, 1, 128, 0, 128, 3, 0, 0, 192, 3, 0, 0, 96, 6, 0, 0, 48, 12, 0, 1, 176, 29, 128, 3, 24, 24, 192, 2, 15, 240, 64, 0, 15, 240, 0, 0, 12, 48, 0, 0, 15, 48, 0, 0, 15, 48, 0, 0, 7, 224, 0, 0, 1, 128, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 0, 0, 0, 3, 0, 0, 192, 1, 135, 241, 128, 0, 28, 121, 0, 0, 112, 14, 0, 0, 96, 6, 0, 0, 192, 3, 0, 1, 128, 1, 0, 1, 128, 1, 128, 1, 128, 1, 128, 57, 128, 0, 188, 1, 128, 1, 128, 1, 128, 1, 128, 1, 128, 1, 128, 0, 128, 3, 0, 0, 192, 3, 0, 0, 96, 6, 0, 0, 48, 12, 0, 1, 176, 29, 128, 3, 24, 24, 192, 2, 15, 240, 64, 0, 15, 240, 0, 0, 12, 48, 0, 0, 15, 48, 0, 0, 15, 48, 0, 0, 7, 224, 0, 0, 1, 128, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 0, 0, 0, 3, 0, 0, 192, 1, 135, 241, 128, 0, 28, 121, 0, 0, 112, 14, 0, 0, 96, 6, 0, 0, 192, 3, 0, 1, 128, 1, 0, 1, 128, 1, 128, 1, 128, 1, 128, 57, 128, 0, 188, 1, 128, 1, 128, 1, 128, 1, 128, 1, 128, 1, 128, 0, 128, 3, 0, 0, 192, 3, 0, 0, 96, 6, 0, 0, 48, 12, 0, 1, 176, 29, 128, 3, 24, 24, 192, 2, 15, 240, 64, 0, 15, 240, 0, 0, 12, 48, 0, 0, 15, 48, 0, 0, 15, 48, 0, 0, 7, 224, 0, 0, 1, 128, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 0, 0, 0, 3, 0, 0, 192, 1, 135, 241, 128, 0, 28, 121, 0, 0, 112, 14, 0, 0, 96, 6, 0, 0, 192, 3, 0, 1, 128, 1, 0, 1, 128, 1, 128, 1, 128, 1, 128, 57, 128, 0, 188, 1, 128, 1, 128, 1, 128, 1, 128, 1, 128, 1, 128, 0, 128, 3, 0, 0, 192, 3, 0, 0, 96, 6, 0, 0, 48, 12, 0, 1, 176, 29, 128, 3, 24, 24, 192, 2, 15, 240, 64, 0, 15, 240, 0, 0, 12, 48, 0, 0, 15, 48, 0, 0, 15, 48, 0, 0, 7, 224, 0, 0, 1, 128, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 0, 0, 0, 3, 0, 0, 192, 1, 135, 241, 128, 0, 28, 121, 0, 0, 112, 14, 0, 0, 96, 6, 0, 0, 192, 3, 0, 1, 128, 1, 0, 1, 128, 1, 128, 1, 128, 1, 128, 57, 128, 0, 188, 1, 128, 1, 128, 1, 128, 1, 128, 1, 128, 1, 128, 0, 128, 3, 0, 0, 192, 3, 0, 0, 96, 6, 0, 0, 48, 12, 0, 1, 176, 29, 128, 3, 24, 24, 192, 2, 15, 240, 64, 0, 15, 240, 0, 0, 12, 48, 0, 0, 15, 48, 0, 0, 15, 48, 0, 0, 7, 224, 0, 0, 1, 128, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 0, 0, 0, 3, 0, 0, 192, 1, 135, 241, 128, 0, 28, 124, 0, 0, 48, 14, 0, 0, 96, 2, 0, 0, 192, 3, 0, 0, 192, 1, 0, 0, 128, 1, 128, 1, 128, 0, 128, 57, 128, 0, 156, 1, 128, 0, 128, 1, 128, 1, 128, 0, 128, 1, 128, 0, 192, 1, 128, 0, 64, 3, 0, 0, 96, 7, 0, 0, 48, 14, 0, 1, 24, 13, 128, 3, 24, 24, 192, 2, 15, 240, 64, 0, 15, 240, 0, 0, 12, 48, 0, 0, 15, 112, 0, 0, 14, 112, 0, 0, 7, 240, 0, 0, 1, 128, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 192, 1, 7, 248, 128, 0, 30, 60, 0, 0, 56, 14, 0, 0, 96, 3, 0, 0, 96, 1, 0, 0, 192, 1, 128, 0, 192, 0, 128, 0, 128, 0, 128, 56, 128, 0, 220, 0, 128, 0, 192, 0, 128, 0, 128, 0, 128, 0, 128, 0, 192, 1, 128, 0, 96, 3, 128, 0, 96, 3, 0, 0, 48, 6, 0, 0, 24, 12, 128, 3, 8, 24, 192, 2, 15, 248, 64, 0, 15, 240, 0, 0, 12, 16, 0, 0, 12, 176, 0, 0, 12, 176, 0, 0, 7, 240, 0, 0, 1, 128, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 192, 0, 7, 248, 0, 0, 14, 60, 0, 0, 56, 6, 0, 0, 112, 3, 0, 0, 96, 1, 128, 0, 192, 0, 128, 0, 192, 0, 128, 0, 192, 0, 192, 48, 128, 0, 204, 0, 128, 0, 192, 0, 192, 0, 192, 0, 192, 0, 128, 0, 64, 1, 128, 0, 96, 1, 128, 0, 48, 3, 0, 0, 48, 6, 0, 0, 24, 12, 0, 2, 8, 24, 192, 2, 15, 248, 64, 0, 15, 240, 0, 0, 12, 16, 0, 0, 14, 240, 0, 0, 14, 48, 0, 0, 7, 240, 0, 0, 1, 128, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 64, 0, 7, 240, 0, 0, 28, 60, 0, 0, 48, 14, 0, 0, 96, 3, 0, 0, 192, 1, 0, 0, 192, 1, 128, 0, 128, 0, 128, 1, 128, 0, 128, 33, 128, 0, 132, 0, 128, 0, 128, 0, 128, 0, 128, 0, 128, 1, 128, 0, 192, 1, 128, 0, 64, 3, 0, 0, 96, 3, 0, 0, 48, 6, 0, 0, 24, 12, 0, 0, 8, 24, 64, 2, 15, 240, 64, 0, 15, 240, 0, 0, 12, 16, 0, 0, 15, 240, 0, 0, 13, 240, 0, 0, 7, 240, 0, 0, 1, 128, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 240, 0, 0, 60, 120, 0, 0, 112, 28, 0, 0, 192, 6, 0, 0, 192, 3, 0, 1, 128, 3, 0, 1, 128, 1, 0, 1, 0, 1, 0, 1, 0, 1, 128, 1, 0, 1, 128, 1, 0, 1, 0, 1, 0, 3, 0, 1, 128, 3, 0, 0, 192, 7, 0, 0, 64, 6, 0, 0, 96, 12, 0, 0, 48, 24, 0, 0, 24, 24, 0, 0, 15, 240, 0, 0, 15, 240, 0, 0, 8, 48, 0, 0, 15, 240, 0, 0, 15, 176, 0, 0, 7, 224, 0, 0, 1, 128, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 224, 0, 0, 56, 248, 0, 0, 96, 28, 0, 0, 192, 12, 0, 1, 128, 6, 0, 1, 128, 2, 0, 3, 0, 3, 0, 3, 0, 1, 0, 3, 0, 1, 0, 3, 0, 1, 0, 3, 0, 3, 0, 1, 0, 3, 0, 1, 128, 3, 0, 1, 128, 6, 0, 0, 192, 14, 0, 0, 96, 12, 0, 0, 48, 24, 0, 0, 24, 48, 0, 0, 15, 240, 0, 0, 15, 240, 0, 0, 8, 48, 0, 0, 15, 176, 0, 0, 15, 176, 0, 0, 7, 224, 0, 0, 1, 128, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 224, 0, 0, 56, 120, 0, 0, 96, 28, 0, 0, 192, 6, 0, 1, 128, 6, 0, 1, 128, 3, 0, 1, 0, 1, 0, 3, 0, 1, 0, 3, 0, 1, 0, 3, 0, 1, 0, 1, 0, 1, 0, 1, 0, 3, 0, 1, 128, 3, 0, 0, 128, 6, 0, 0, 192, 6, 0, 0, 96, 12, 0, 0, 48, 24, 0, 0, 24, 16, 0, 0, 15, 240, 0, 0, 15, 240, 0, 0, 8, 48, 0, 0, 15, 240, 0, 0, 15, 240, 0, 0, 7, 224, 0, 0, 1, 128, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 240, 0, 0, 28, 120, 0, 0, 112, 12, 0, 0, 192, 6, 0, 0, 192, 3, 0, 1, 128, 1, 0, 1, 128, 1, 0, 1, 128, 1, 128, 1, 0, 1, 128, 1, 0, 1, 128, 1, 0, 1, 128, 1, 128, 1, 0, 0, 128, 3, 0, 0, 192, 3, 0, 0, 96, 6, 0, 0, 96, 12, 0, 0, 48, 24, 0, 0, 24, 24, 0, 0, 15, 240, 0, 0, 15, 240, 0, 0, 12, 48, 0, 0, 13, 240, 0, 0, 13, 240, 0, 0, 7, 224, 0, 0, 1, 128, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 240, 0, 0, 28, 120, 0, 0, 112, 14, 0, 0, 96, 6, 0, 0, 192, 3, 0, 1, 128, 1, 0, 1, 128, 1, 128, 1, 128, 1, 128, 1, 128, 0, 128, 1, 128, 1, 128, 1, 128, 1, 128, 1, 128, 1, 128, 0, 128, 3, 0, 0, 192, 3, 0, 0, 96, 6, 0, 0, 48, 12, 0, 0, 48, 28, 0, 0, 24, 24, 0, 0, 15, 240, 0, 0, 15, 240, 0, 0, 12, 48, 0, 0, 13, 240, 0, 0, 13, 240, 0, 0, 7, 224, 0, 0, 1, 128, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 240, 0, 0, 28, 120, 0, 0, 112, 14, 0, 0, 96, 6, 0, 0, 192, 3, 0, 1, 128, 1, 0, 1, 128, 1, 128, 1, 128, 1, 128, 1, 128, 0, 128, 1, 128, 1, 128, 1, 128, 1, 128, 1, 128, 1, 128, 0, 128, 3, 0, 0, 192, 3, 0, 0, 96, 6, 0, 0, 48, 12, 0, 0, 48, 28, 0, 0, 24, 24, 0, 0, 15, 240, 0, 0, 15, 240, 0, 0, 12, 48, 0, 0, 13, 240, 0, 0, 13, 240, 0, 0, 7, 224, 0, 0, 1, 128, 0, 0, 0, 0, 0 }
  };
  int y_mid = 10;
  if (pos == 0) {
    display.fillRoundRect(UI_MIDDLE_X - 2 - 42, UI_MIDDLE_Y - 2, 36, 36, 10, 0);
    display.drawBitmap(UI_MIDDLE_X - 42, UI_MIDDLE_Y, frames[0], FRAME_WIDTH, FRAME_HEIGHT, 1);
  }
  if (pos == 1) {
    display.fillRoundRect(UI_MIDDLE_X - 2, UI_MIDDLE_Y - 2, 36, 36, 10, 0);
    display.drawBitmap(UI_MIDDLE_X, UI_MIDDLE_Y, frames[0], FRAME_WIDTH, FRAME_HEIGHT, 1);
    display.display();
    for (int frame = 0; frame < 28; frame++) {
      if (frame < 14 && !(frame % 7)) {
        // clears previous display
        display.fillRoundRect(UI_MIDDLE_X - 4, y_mid - 4, 40, 40, 10, 0);
        // change display pos for next movement
        y_mid++;
        // fills in with new display
        display.fillRoundRect(UI_MIDDLE_X - 4, y_mid - 4, 40, 40, 10, 1);
        display.fillRoundRect(UI_MIDDLE_X - 2, y_mid - 2, 36, 36, 10, 0);
        // fills in icon onto display
        display.drawBitmap(UI_MIDDLE_X, y_mid, frames[frame], FRAME_WIDTH, FRAME_HEIGHT, 1);
        display.display();
      }
      if (frame >= 14 && !(frame % 7)) {
        display.fillRoundRect(UI_MIDDLE_X - 4, y_mid - 4, 40, 40, 10, 0);
        y_mid--;
        display.fillRoundRect(UI_MIDDLE_X - 4, y_mid - 4, 40, 40, 10, 1);
        display.fillRoundRect(UI_MIDDLE_X - 2, y_mid - 2, 36, 36, 10, 0);
        display.drawBitmap(UI_MIDDLE_X, y_mid, frames[frame], FRAME_WIDTH, FRAME_HEIGHT, 1);
        display.display();
      } else {
        display.fillRoundRect(UI_MIDDLE_X - 4, y_mid - 4, 40, 40, 10, 0);
        display.fillRoundRect(UI_MIDDLE_X - 4, y_mid - 4, 40, 40, 10, 1);
        display.fillRoundRect(UI_MIDDLE_X - 2, y_mid - 2, 36, 36, 10, 0);
        display.drawBitmap(UI_MIDDLE_X, y_mid, frames[frame], FRAME_WIDTH, FRAME_HEIGHT, 1);
        display.display();
      }
    }
  }
  if (pos == 2) {
    display.fillRoundRect(UI_MIDDLE_X - 2 + 42, UI_MIDDLE_Y - 2, 36, 36, 10, 0);
    display.drawBitmap(UI_MIDDLE_X + 42, UI_MIDDLE_Y, frames[0], FRAME_WIDTH, FRAME_HEIGHT, 1);
  }
}

void UI_cabinet(int pos) {
  const byte PROGMEM frames[][128] = {
    { 0, 0, 31, 128, 0, 0, 127, 224, 0, 1, 192, 112, 0, 1, 128, 56, 0, 3, 31, 140, 0, 6, 49, 196, 0, 6, 96, 102, 0, 6, 64, 50, 0, 6, 64, 19, 0, 4, 64, 17, 0, 6, 64, 19, 0, 2, 96, 51, 0, 3, 48, 51, 0, 3, 28, 99, 0, 3, 143, 198, 0, 6, 192, 12, 0, 12, 112, 28, 0, 24, 254, 112, 0, 49, 143, 192, 0, 99, 0, 0, 0, 198, 0, 0, 61, 140, 0, 0, 111, 24, 0, 0, 198, 48, 0, 0, 204, 96, 0, 0, 248, 192, 0, 0, 113, 128, 0, 0, 99, 0, 0, 0, 198, 0, 0, 0, 204, 0, 0, 0, 248, 0, 0, 0, 112, 0, 0, 0 },
    { 0, 0, 31, 128, 0, 0, 127, 224, 0, 1, 192, 112, 0, 1, 128, 56, 0, 3, 31, 140, 0, 6, 49, 196, 0, 6, 96, 102, 0, 6, 64, 50, 0, 6, 64, 19, 0, 4, 64, 17, 0, 6, 64, 19, 0, 2, 96, 51, 0, 3, 48, 51, 0, 3, 28, 99, 0, 3, 143, 198, 0, 6, 192, 12, 0, 12, 112, 28, 0, 24, 254, 112, 0, 49, 143, 192, 0, 99, 0, 0, 0, 198, 0, 0, 61, 140, 0, 0, 111, 24, 0, 0, 198, 48, 0, 0, 204, 96, 0, 0, 248, 192, 0, 0, 113, 128, 0, 0, 99, 0, 0, 0, 198, 0, 0, 0, 204, 0, 0, 0, 248, 0, 0, 0, 112, 0, 0, 0 },
    { 0, 0, 31, 128, 0, 0, 127, 224, 0, 1, 192, 112, 0, 1, 128, 56, 0, 3, 31, 140, 0, 6, 49, 196, 0, 6, 96, 102, 0, 6, 64, 50, 0, 6, 64, 19, 0, 4, 64, 17, 0, 6, 64, 19, 0, 2, 96, 51, 0, 3, 48, 51, 0, 3, 28, 99, 0, 3, 143, 198, 0, 6, 192, 12, 0, 12, 112, 28, 0, 24, 254, 112, 0, 49, 143, 192, 0, 99, 0, 0, 0, 198, 0, 0, 61, 140, 0, 0, 111, 24, 0, 0, 198, 48, 0, 0, 204, 96, 0, 0, 248, 192, 0, 0, 113, 128, 0, 0, 99, 0, 0, 0, 198, 0, 0, 0, 204, 0, 0, 0, 248, 0, 0, 0, 112, 0, 0, 0 },
    { 0, 0, 31, 128, 0, 0, 127, 224, 0, 1, 192, 112, 0, 1, 128, 56, 0, 3, 31, 140, 0, 6, 49, 196, 0, 6, 96, 102, 0, 6, 64, 50, 0, 6, 64, 19, 0, 4, 64, 17, 0, 6, 64, 19, 0, 2, 96, 51, 0, 3, 48, 51, 0, 3, 28, 99, 0, 3, 143, 198, 0, 6, 192, 12, 0, 12, 112, 28, 0, 24, 254, 112, 0, 49, 143, 192, 0, 99, 0, 0, 0, 198, 0, 0, 61, 140, 0, 0, 111, 24, 0, 0, 198, 48, 0, 0, 204, 96, 0, 0, 248, 192, 0, 0, 113, 128, 0, 0, 99, 0, 0, 0, 198, 0, 0, 0, 204, 0, 0, 0, 248, 0, 0, 0, 112, 0, 0, 0 },
    { 0, 0, 2, 0, 0, 0, 63, 192, 0, 0, 96, 112, 0, 0, 192, 56, 0, 1, 131, 140, 0, 3, 31, 196, 0, 3, 56, 230, 0, 2, 48, 34, 0, 6, 96, 50, 0, 6, 96, 51, 0, 2, 96, 51, 0, 2, 32, 98, 0, 3, 48, 230, 0, 3, 31, 198, 0, 3, 142, 12, 0, 6, 192, 24, 0, 12, 112, 48, 0, 24, 255, 224, 0, 49, 135, 0, 0, 99, 0, 0, 0, 198, 0, 0, 29, 140, 0, 0, 127, 24, 0, 0, 102, 48, 0, 0, 204, 96, 0, 0, 120, 192, 0, 0, 113, 128, 0, 0, 99, 0, 0, 0, 198, 0, 0, 0, 204, 0, 0, 0, 248, 0, 0, 0, 112, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 3, 192, 0, 0, 15, 240, 0, 0, 24, 56, 0, 0, 112, 12, 0, 0, 225, 196, 0, 0, 199, 198, 0, 1, 140, 102, 0, 1, 152, 102, 0, 3, 24, 198, 0, 3, 48, 204, 0, 3, 49, 140, 0, 3, 31, 24, 0, 3, 28, 56, 0, 3, 128, 112, 0, 6, 224, 224, 0, 12, 127, 128, 0, 24, 254, 0, 0, 49, 128, 0, 0, 99, 0, 0, 0, 198, 0, 0, 13, 140, 0, 0, 31, 24, 0, 0, 54, 48, 0, 0, 108, 96, 0, 0, 120, 192, 0, 0, 113, 128, 0, 0, 99, 0, 0, 0, 198, 0, 0, 0, 204, 0, 0, 0, 248, 0, 0, 0, 112, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 240, 0, 0, 3, 56, 0, 0, 14, 12, 0, 0, 28, 4, 0, 0, 48, 4, 0, 0, 112, 12, 0, 0, 96, 12, 0, 0, 192, 24, 0, 1, 128, 48, 0, 1, 128, 112, 0, 1, 0, 96, 0, 1, 1, 192, 0, 3, 131, 128, 0, 6, 230, 0, 0, 12, 124, 0, 0, 24, 192, 0, 0, 49, 128, 0, 0, 99, 0, 0, 0, 198, 0, 0, 1, 140, 0, 0, 15, 24, 0, 0, 30, 48, 0, 0, 60, 96, 0, 0, 120, 192, 0, 0, 113, 128, 0, 0, 99, 0, 0, 0, 198, 0, 0, 0, 204, 0, 0, 0, 248, 0, 0, 0, 112, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0, 0, 248, 0, 0, 1, 200, 0, 0, 3, 140, 0, 0, 7, 24, 0, 0, 14, 56, 0, 0, 28, 112, 0, 0, 56, 224, 0, 0, 113, 192, 0, 0, 227, 128, 0, 1, 199, 0, 0, 1, 142, 0, 0, 3, 28, 0, 0, 6, 248, 0, 0, 12, 224, 0, 0, 24, 192, 0, 0, 49, 128, 0, 0, 99, 0, 0, 0, 198, 0, 0, 1, 140, 0, 0, 3, 24, 0, 0, 6, 48, 0, 0, 12, 96, 0, 0, 24, 192, 0, 0, 49, 128, 0, 0, 99, 0, 0, 0, 198, 0, 0, 0, 204, 0, 0, 0, 248, 0, 0, 0, 112, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 240, 0, 0, 3, 56, 0, 0, 14, 12, 0, 0, 28, 4, 0, 0, 48, 196, 0, 0, 113, 204, 0, 0, 99, 140, 0, 0, 199, 24, 0, 1, 142, 48, 0, 1, 156, 112, 0, 1, 24, 96, 0, 1, 1, 192, 0, 3, 131, 128, 0, 6, 230, 0, 0, 12, 124, 0, 0, 24, 192, 0, 0, 49, 128, 0, 0, 99, 0, 0, 0, 198, 0, 0, 1, 140, 0, 0, 3, 24, 0, 0, 6, 48, 0, 0, 12, 96, 0, 0, 24, 192, 0, 0, 49, 192, 0, 0, 99, 192, 0, 0, 199, 128, 0, 0, 207, 0, 0, 0, 252, 0, 0, 0, 112, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 3, 192, 0, 0, 15, 240, 0, 0, 24, 56, 0, 0, 112, 12, 0, 0, 225, 196, 0, 0, 199, 198, 0, 1, 140, 102, 0, 1, 152, 102, 0, 3, 24, 198, 0, 3, 48, 204, 0, 3, 49, 140, 0, 3, 63, 24, 0, 3, 28, 56, 0, 3, 128, 112, 0, 6, 224, 224, 0, 12, 127, 128, 0, 24, 254, 0, 0, 49, 128, 0, 0, 99, 0, 0, 0, 198, 0, 0, 1, 140, 0, 0, 3, 24, 0, 0, 6, 48, 0, 0, 12, 96, 0, 0, 24, 192, 0, 0, 49, 192, 0, 0, 99, 96, 0, 0, 198, 192, 0, 0, 207, 128, 0, 0, 255, 0, 0, 0, 112, 0, 0, 0 },
    { 0, 0, 2, 0, 0, 0, 63, 192, 0, 0, 96, 112, 0, 0, 192, 56, 0, 1, 135, 140, 0, 3, 31, 196, 0, 3, 48, 230, 0, 2, 48, 34, 0, 6, 96, 50, 0, 6, 96, 51, 0, 2, 96, 51, 0, 2, 32, 98, 0, 3, 48, 102, 0, 3, 31, 198, 0, 3, 143, 12, 0, 6, 192, 24, 0, 12, 112, 48, 0, 24, 255, 224, 0, 49, 135, 0, 0, 99, 0, 0, 0, 198, 0, 0, 1, 140, 0, 0, 3, 24, 0, 0, 6, 48, 0, 0, 12, 96, 0, 0, 24, 192, 0, 0, 49, 224, 0, 0, 99, 96, 0, 0, 198, 96, 0, 0, 206, 224, 0, 0, 255, 192, 0, 0, 115, 0, 0, 0 },
    { 0, 0, 31, 128, 0, 0, 127, 224, 0, 1, 192, 112, 0, 1, 128, 56, 0, 3, 31, 140, 0, 6, 49, 196, 0, 6, 96, 102, 0, 6, 64, 50, 0, 6, 64, 19, 0, 4, 64, 17, 0, 6, 64, 19, 0, 2, 96, 51, 0, 3, 48, 51, 0, 3, 28, 99, 0, 3, 143, 198, 0, 6, 192, 12, 0, 12, 112, 28, 0, 24, 254, 112, 0, 49, 143, 192, 0, 99, 0, 0, 0, 198, 0, 0, 1, 140, 0, 0, 3, 24, 0, 0, 6, 48, 0, 0, 12, 96, 0, 0, 24, 192, 0, 0, 49, 224, 0, 0, 99, 96, 0, 0, 198, 48, 0, 0, 206, 96, 0, 0, 255, 192, 0, 0, 115, 128, 0, 0 },
    { 0, 0, 31, 128, 0, 0, 127, 224, 0, 1, 192, 112, 0, 1, 128, 56, 0, 3, 31, 140, 0, 6, 49, 196, 0, 6, 96, 102, 0, 6, 64, 50, 0, 6, 64, 19, 0, 4, 64, 17, 0, 6, 64, 19, 0, 2, 96, 51, 0, 3, 48, 51, 0, 3, 28, 99, 0, 3, 143, 198, 0, 6, 192, 12, 0, 12, 112, 28, 0, 24, 254, 112, 0, 49, 143, 192, 0, 99, 0, 0, 0, 198, 0, 0, 1, 140, 0, 0, 3, 24, 0, 0, 6, 48, 0, 0, 12, 96, 0, 0, 24, 192, 0, 0, 49, 224, 0, 0, 99, 96, 0, 0, 198, 48, 0, 0, 206, 96, 0, 0, 255, 192, 0, 0, 115, 128, 0, 0 },
    { 0, 0, 31, 128, 0, 0, 127, 224, 0, 1, 192, 112, 0, 1, 128, 56, 0, 3, 31, 140, 0, 6, 49, 196, 0, 6, 96, 102, 0, 6, 64, 50, 0, 6, 64, 19, 0, 4, 64, 17, 0, 6, 64, 19, 0, 2, 96, 51, 0, 3, 48, 51, 0, 3, 28, 99, 0, 3, 143, 198, 0, 6, 192, 12, 0, 12, 112, 28, 0, 24, 254, 112, 0, 49, 143, 192, 0, 99, 0, 0, 0, 198, 0, 0, 1, 140, 0, 0, 3, 24, 0, 0, 6, 48, 0, 0, 12, 96, 0, 0, 24, 192, 0, 0, 49, 224, 0, 0, 99, 96, 0, 0, 198, 48, 0, 0, 206, 96, 0, 0, 255, 192, 0, 0, 115, 128, 0, 0 },
    { 0, 0, 31, 128, 0, 0, 127, 224, 0, 1, 192, 112, 0, 1, 128, 56, 0, 3, 31, 140, 0, 6, 49, 196, 0, 6, 96, 102, 0, 6, 64, 50, 0, 6, 64, 19, 0, 4, 64, 17, 0, 6, 64, 19, 0, 2, 96, 51, 0, 3, 48, 51, 0, 3, 28, 99, 0, 3, 143, 198, 0, 6, 192, 12, 0, 12, 112, 28, 0, 24, 254, 112, 0, 49, 143, 192, 0, 99, 0, 0, 0, 198, 0, 0, 1, 140, 0, 0, 3, 24, 0, 0, 6, 48, 0, 0, 12, 96, 0, 0, 24, 192, 0, 0, 49, 224, 0, 0, 99, 96, 0, 0, 198, 48, 0, 0, 206, 96, 0, 0, 255, 192, 0, 0, 115, 128, 0, 0 },
    { 0, 0, 31, 128, 0, 0, 127, 224, 0, 1, 192, 112, 0, 1, 128, 56, 0, 3, 31, 140, 0, 6, 49, 196, 0, 6, 96, 102, 0, 6, 64, 50, 0, 6, 64, 19, 0, 4, 64, 17, 0, 6, 64, 19, 0, 2, 96, 51, 0, 3, 48, 51, 0, 3, 28, 99, 0, 3, 143, 198, 0, 6, 192, 12, 0, 12, 112, 28, 0, 24, 254, 112, 0, 49, 143, 192, 0, 99, 0, 0, 0, 198, 0, 0, 1, 140, 0, 0, 3, 24, 0, 0, 6, 48, 0, 0, 12, 96, 0, 0, 24, 192, 0, 0, 49, 224, 0, 0, 99, 96, 0, 0, 198, 48, 0, 0, 206, 96, 0, 0, 255, 192, 0, 0, 115, 128, 0, 0 },
    { 0, 0, 31, 128, 0, 0, 127, 224, 0, 1, 192, 112, 0, 1, 128, 56, 0, 3, 31, 140, 0, 6, 49, 196, 0, 6, 96, 102, 0, 6, 64, 50, 0, 6, 64, 19, 0, 4, 64, 17, 0, 6, 64, 19, 0, 2, 96, 51, 0, 3, 48, 51, 0, 3, 28, 99, 0, 3, 143, 198, 0, 6, 192, 12, 0, 12, 112, 28, 0, 24, 254, 112, 0, 49, 143, 192, 0, 99, 0, 0, 0, 198, 0, 0, 1, 140, 0, 0, 3, 24, 0, 0, 6, 48, 0, 0, 12, 96, 0, 0, 24, 192, 0, 0, 49, 224, 0, 0, 99, 96, 0, 0, 198, 48, 0, 0, 206, 96, 0, 0, 255, 192, 0, 0, 115, 128, 0, 0 },
    { 0, 0, 31, 128, 0, 0, 127, 224, 0, 1, 192, 112, 0, 1, 128, 56, 0, 3, 31, 140, 0, 6, 49, 196, 0, 6, 96, 102, 0, 6, 64, 50, 0, 6, 64, 19, 0, 4, 64, 17, 0, 6, 64, 19, 0, 2, 96, 51, 0, 3, 48, 51, 0, 3, 28, 99, 0, 3, 143, 198, 0, 6, 192, 12, 0, 12, 112, 28, 0, 24, 254, 112, 0, 49, 143, 192, 0, 99, 0, 0, 0, 198, 0, 0, 1, 140, 0, 0, 3, 24, 0, 0, 6, 48, 0, 0, 12, 96, 0, 0, 24, 192, 0, 0, 49, 224, 0, 0, 99, 96, 0, 0, 198, 48, 0, 0, 206, 96, 0, 0, 255, 192, 0, 0, 115, 128, 0, 0 },
    { 0, 0, 2, 0, 0, 0, 63, 192, 0, 0, 96, 112, 0, 0, 192, 56, 0, 1, 135, 140, 0, 3, 31, 196, 0, 3, 48, 230, 0, 2, 48, 34, 0, 6, 96, 50, 0, 6, 96, 51, 0, 2, 96, 51, 0, 2, 32, 98, 0, 3, 48, 102, 0, 3, 31, 198, 0, 3, 143, 12, 0, 6, 192, 24, 0, 12, 112, 48, 0, 24, 255, 224, 0, 49, 135, 0, 0, 99, 0, 0, 0, 198, 0, 0, 1, 140, 0, 0, 3, 24, 0, 0, 6, 48, 0, 0, 12, 96, 0, 0, 24, 192, 0, 0, 49, 224, 0, 0, 99, 96, 0, 0, 198, 96, 0, 0, 206, 224, 0, 0, 255, 192, 0, 0, 115, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 3, 192, 0, 0, 15, 240, 0, 0, 24, 56, 0, 0, 112, 12, 0, 0, 225, 196, 0, 0, 199, 198, 0, 1, 140, 102, 0, 1, 152, 102, 0, 3, 24, 198, 0, 3, 48, 204, 0, 3, 49, 140, 0, 3, 63, 24, 0, 3, 28, 56, 0, 3, 128, 112, 0, 6, 224, 224, 0, 12, 127, 128, 0, 24, 254, 0, 0, 49, 128, 0, 0, 99, 0, 0, 0, 198, 0, 0, 1, 140, 0, 0, 3, 24, 0, 0, 6, 48, 0, 0, 12, 96, 0, 0, 24, 192, 0, 0, 49, 192, 0, 0, 99, 96, 0, 0, 198, 192, 0, 0, 207, 128, 0, 0, 255, 0, 0, 0, 112, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 240, 0, 0, 3, 56, 0, 0, 14, 12, 0, 0, 28, 4, 0, 0, 48, 4, 0, 0, 112, 12, 0, 0, 96, 12, 0, 0, 192, 24, 0, 1, 128, 48, 0, 1, 128, 112, 0, 1, 0, 96, 0, 1, 1, 192, 0, 3, 131, 128, 0, 6, 230, 0, 0, 12, 124, 0, 0, 24, 192, 0, 0, 49, 128, 0, 0, 99, 0, 0, 0, 198, 0, 0, 1, 140, 0, 0, 3, 24, 0, 0, 6, 48, 0, 0, 12, 96, 0, 0, 24, 192, 0, 0, 49, 192, 0, 0, 99, 192, 0, 0, 199, 128, 0, 0, 207, 0, 0, 0, 252, 0, 0, 0, 112, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0, 0, 248, 0, 0, 1, 200, 0, 0, 3, 140, 0, 0, 7, 24, 0, 0, 14, 56, 0, 0, 28, 112, 0, 0, 56, 224, 0, 0, 113, 192, 0, 0, 227, 128, 0, 1, 199, 0, 0, 1, 142, 0, 0, 3, 28, 0, 0, 6, 248, 0, 0, 12, 224, 0, 0, 24, 192, 0, 0, 49, 128, 0, 0, 99, 0, 0, 0, 198, 0, 0, 1, 140, 0, 0, 3, 24, 0, 0, 6, 48, 0, 0, 12, 96, 0, 0, 24, 192, 0, 0, 49, 128, 0, 0, 99, 0, 0, 0, 198, 0, 0, 0, 204, 0, 0, 0, 248, 0, 0, 0, 112, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 240, 0, 0, 3, 56, 0, 0, 14, 12, 0, 0, 28, 4, 0, 0, 48, 196, 0, 0, 113, 204, 0, 0, 99, 140, 0, 0, 199, 24, 0, 1, 142, 48, 0, 1, 156, 112, 0, 1, 24, 96, 0, 1, 1, 192, 0, 3, 131, 128, 0, 6, 230, 0, 0, 12, 124, 0, 0, 24, 192, 0, 0, 49, 128, 0, 0, 99, 0, 0, 0, 198, 0, 0, 1, 140, 0, 0, 15, 24, 0, 0, 30, 48, 0, 0, 60, 96, 0, 0, 120, 192, 0, 0, 113, 128, 0, 0, 99, 0, 0, 0, 198, 0, 0, 0, 204, 0, 0, 0, 248, 0, 0, 0, 112, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 3, 192, 0, 0, 15, 240, 0, 0, 24, 56, 0, 0, 112, 12, 0, 0, 225, 196, 0, 0, 199, 198, 0, 1, 140, 102, 0, 1, 152, 102, 0, 3, 24, 198, 0, 3, 48, 204, 0, 3, 49, 140, 0, 3, 31, 24, 0, 3, 28, 56, 0, 3, 128, 112, 0, 6, 224, 224, 0, 12, 127, 128, 0, 24, 254, 0, 0, 49, 128, 0, 0, 99, 0, 0, 0, 198, 0, 0, 13, 140, 0, 0, 31, 24, 0, 0, 54, 48, 0, 0, 108, 96, 0, 0, 120, 192, 0, 0, 113, 128, 0, 0, 99, 0, 0, 0, 198, 0, 0, 0, 204, 0, 0, 0, 248, 0, 0, 0, 112, 0, 0, 0 },
    { 0, 0, 2, 0, 0, 0, 63, 192, 0, 0, 96, 112, 0, 0, 192, 56, 0, 1, 131, 140, 0, 3, 31, 196, 0, 3, 56, 230, 0, 2, 48, 34, 0, 6, 96, 50, 0, 6, 96, 51, 0, 2, 96, 51, 0, 2, 32, 98, 0, 3, 48, 230, 0, 3, 31, 198, 0, 3, 142, 12, 0, 6, 192, 24, 0, 12, 112, 48, 0, 24, 255, 224, 0, 49, 135, 0, 0, 99, 0, 0, 0, 198, 0, 0, 29, 140, 0, 0, 127, 24, 0, 0, 102, 48, 0, 0, 204, 96, 0, 0, 120, 192, 0, 0, 113, 128, 0, 0, 99, 0, 0, 0, 198, 0, 0, 0, 204, 0, 0, 0, 248, 0, 0, 0, 112, 0, 0, 0 },
    { 0, 0, 31, 128, 0, 0, 127, 224, 0, 1, 192, 112, 0, 1, 128, 56, 0, 3, 31, 140, 0, 6, 49, 196, 0, 6, 96, 102, 0, 6, 64, 50, 0, 6, 64, 19, 0, 4, 64, 17, 0, 6, 64, 19, 0, 2, 96, 51, 0, 3, 48, 51, 0, 3, 28, 99, 0, 3, 143, 198, 0, 6, 192, 12, 0, 12, 112, 28, 0, 24, 254, 112, 0, 49, 143, 192, 0, 99, 0, 0, 0, 198, 0, 0, 61, 140, 0, 0, 111, 24, 0, 0, 198, 48, 0, 0, 204, 96, 0, 0, 248, 192, 0, 0, 113, 128, 0, 0, 99, 0, 0, 0, 198, 0, 0, 0, 204, 0, 0, 0, 248, 0, 0, 0, 112, 0, 0, 0 },
    { 0, 0, 31, 128, 0, 0, 127, 224, 0, 1, 192, 112, 0, 1, 128, 56, 0, 3, 31, 140, 0, 6, 49, 196, 0, 6, 96, 102, 0, 6, 64, 50, 0, 6, 64, 19, 0, 4, 64, 17, 0, 6, 64, 19, 0, 2, 96, 51, 0, 3, 48, 51, 0, 3, 28, 99, 0, 3, 143, 198, 0, 6, 192, 12, 0, 12, 112, 28, 0, 24, 254, 112, 0, 49, 143, 192, 0, 99, 0, 0, 0, 198, 0, 0, 61, 140, 0, 0, 111, 24, 0, 0, 198, 48, 0, 0, 204, 96, 0, 0, 248, 192, 0, 0, 113, 128, 0, 0, 99, 0, 0, 0, 198, 0, 0, 0, 204, 0, 0, 0, 248, 0, 0, 0, 112, 0, 0, 0 },
    { 0, 0, 31, 128, 0, 0, 127, 224, 0, 1, 192, 112, 0, 1, 128, 56, 0, 3, 31, 140, 0, 6, 49, 196, 0, 6, 96, 102, 0, 6, 64, 50, 0, 6, 64, 19, 0, 4, 64, 17, 0, 6, 64, 19, 0, 2, 96, 51, 0, 3, 48, 51, 0, 3, 28, 99, 0, 3, 143, 198, 0, 6, 192, 12, 0, 12, 112, 28, 0, 24, 254, 112, 0, 49, 143, 192, 0, 99, 0, 0, 0, 198, 0, 0, 61, 140, 0, 0, 111, 24, 0, 0, 198, 48, 0, 0, 204, 96, 0, 0, 248, 192, 0, 0, 113, 128, 0, 0, 99, 0, 0, 0, 198, 0, 0, 0, 204, 0, 0, 0, 248, 0, 0, 0, 112, 0, 0, 0 }
  };
  int y_mid = 10;
  if (pos == 0) {
    display.fillRoundRect(UI_MIDDLE_X - 2 - 42, UI_MIDDLE_Y - 2, 36, 36, 10, 0);
    display.drawBitmap(UI_MIDDLE_X - 42, UI_MIDDLE_Y, frames[0], FRAME_WIDTH, FRAME_HEIGHT, 1);
  }
  if (pos == 1) {
    display.fillRoundRect(UI_MIDDLE_X - 2, UI_MIDDLE_Y - 2, 36, 36, 10, 0);
    display.drawBitmap(UI_MIDDLE_X, UI_MIDDLE_Y, frames[0], FRAME_WIDTH, FRAME_HEIGHT, 1);
    display.display();
    for (int frame = 0; frame < 28; frame++) {
      if (frame < 14 && !(frame % 7)) {
        // clears previous display
        display.fillRoundRect(UI_MIDDLE_X - 4, y_mid - 4, 40, 40, 10, 0);
        // change display pos for next movement
        y_mid++;
        // fills in with new display
        display.fillRoundRect(UI_MIDDLE_X - 4, y_mid - 4, 40, 40, 10, 1);
        display.fillRoundRect(UI_MIDDLE_X - 2, y_mid - 2, 36, 36, 10, 0);
        // fills in icon onto display
        display.drawBitmap(UI_MIDDLE_X, y_mid, frames[frame], FRAME_WIDTH, FRAME_HEIGHT, 1);
        display.display();
      }
      if (frame >= 14 && !(frame % 7)) {
        display.fillRoundRect(UI_MIDDLE_X - 4, y_mid - 4, 40, 40, 10, 0);
        y_mid--;
        display.fillRoundRect(UI_MIDDLE_X - 4, y_mid - 4, 40, 40, 10, 1);
        display.fillRoundRect(UI_MIDDLE_X - 2, y_mid - 2, 36, 36, 10, 0);
        display.drawBitmap(UI_MIDDLE_X, y_mid, frames[frame], FRAME_WIDTH, FRAME_HEIGHT, 1);
        display.display();
      } else {
        display.fillRoundRect(UI_MIDDLE_X - 4, y_mid - 4, 40, 40, 10, 0);
        display.fillRoundRect(UI_MIDDLE_X - 4, y_mid - 4, 40, 40, 10, 1);
        display.fillRoundRect(UI_MIDDLE_X - 2, y_mid - 2, 36, 36, 10, 0);
        display.drawBitmap(UI_MIDDLE_X, y_mid, frames[frame], FRAME_WIDTH, FRAME_HEIGHT, 1);
        display.display();
      }
    }
  }
  if (pos == 2) {
    display.fillRoundRect(UI_MIDDLE_X - 2 + 42, UI_MIDDLE_Y - 2, 36, 36, 10, 0);
    display.drawBitmap(UI_MIDDLE_X + 42, UI_MIDDLE_Y, frames[0], FRAME_WIDTH, FRAME_HEIGHT, 1);
  }
}

void UI_cancel(int pos) {
  const byte PROGMEM frames[][128] = {
    { 0, 0, 0, 0, 0, 3, 224, 0, 0, 63, 252, 0, 0, 224, 15, 0, 1, 128, 3, 128, 3, 0, 0, 192, 6, 0, 0, 96, 12, 0, 0, 48, 24, 0, 0, 24, 24, 1, 128, 8, 48, 1, 128, 12, 48, 1, 128, 4, 32, 1, 128, 4, 96, 1, 128, 4, 96, 1, 128, 6, 96, 1, 128, 6, 96, 1, 128, 6, 96, 1, 128, 6, 32, 0, 0, 6, 32, 0, 0, 4, 32, 0, 0, 12, 48, 1, 128, 12, 16, 1, 128, 24, 24, 0, 0, 24, 12, 0, 0, 48, 6, 0, 0, 96, 3, 0, 0, 192, 1, 192, 1, 128, 0, 240, 7, 0, 0, 63, 252, 0, 0, 7, 192, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 1, 0, 0, 0, 31, 120, 0, 0, 96, 6, 0, 1, 128, 1, 128, 2, 0, 0, 64, 4, 0, 0, 32, 8, 0, 0, 16, 8, 0, 0, 16, 16, 1, 128, 8, 16, 1, 128, 8, 32, 1, 128, 4, 32, 1, 128, 4, 32, 1, 128, 4, 32, 1, 128, 4, 0, 1, 128, 6, 96, 1, 128, 0, 32, 1, 128, 4, 32, 0, 0, 4, 32, 0, 0, 4, 32, 0, 0, 4, 16, 1, 128, 8, 16, 1, 128, 8, 8, 0, 0, 16, 8, 0, 0, 16, 4, 0, 0, 32, 2, 0, 0, 64, 1, 128, 1, 128, 0, 96, 6, 0, 0, 30, 248, 0, 0, 0, 128, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 1, 128, 0, 0, 63, 120, 0, 0, 96, 6, 0, 1, 128, 1, 128, 2, 0, 0, 64, 4, 0, 0, 32, 8, 0, 0, 16, 8, 0, 0, 16, 16, 1, 128, 8, 16, 1, 128, 12, 32, 1, 128, 4, 32, 1, 128, 4, 32, 1, 128, 4, 32, 1, 128, 4, 64, 1, 128, 6, 96, 1, 128, 2, 32, 1, 128, 4, 32, 0, 0, 4, 32, 0, 0, 4, 32, 0, 0, 4, 48, 1, 128, 8, 16, 1, 128, 8, 8, 0, 0, 16, 8, 0, 0, 16, 4, 0, 0, 32, 2, 0, 0, 64, 1, 128, 1, 128, 0, 96, 6, 0, 0, 30, 252, 0, 0, 1, 128, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 3, 128, 0, 0, 62, 120, 0, 0, 224, 2, 0, 1, 128, 0, 128, 2, 0, 0, 64, 4, 0, 0, 32, 8, 0, 0, 16, 0, 0, 0, 24, 16, 1, 128, 8, 0, 1, 128, 12, 32, 1, 128, 4, 32, 1, 128, 4, 32, 1, 128, 4, 32, 1, 128, 6, 64, 1, 128, 2, 64, 1, 128, 2, 96, 1, 128, 4, 32, 0, 0, 4, 32, 0, 0, 4, 32, 0, 0, 4, 48, 1, 128, 0, 16, 1, 128, 8, 24, 0, 0, 0, 8, 0, 0, 16, 4, 0, 0, 32, 2, 0, 0, 64, 1, 0, 1, 128, 0, 64, 7, 0, 0, 30, 124, 0, 0, 1, 192, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 3, 128, 0, 0, 62, 56, 0, 0, 224, 2, 0, 1, 128, 0, 128, 2, 0, 0, 64, 4, 0, 0, 32, 8, 0, 0, 16, 0, 0, 0, 24, 16, 1, 128, 8, 0, 1, 128, 12, 32, 1, 128, 4, 32, 1, 128, 4, 32, 1, 128, 4, 0, 1, 128, 6, 64, 1, 128, 2, 64, 1, 128, 2, 96, 1, 128, 0, 32, 0, 0, 4, 32, 0, 0, 4, 32, 0, 0, 4, 48, 1, 128, 0, 16, 1, 128, 8, 24, 0, 0, 0, 8, 0, 0, 16, 4, 0, 0, 32, 2, 0, 0, 64, 1, 0, 1, 128, 0, 64, 7, 0, 0, 28, 124, 0, 0, 1, 192, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 7, 192, 0, 0, 60, 28, 0, 0, 192, 3, 0, 1, 128, 0, 128, 2, 0, 0, 64, 4, 0, 0, 32, 8, 0, 0, 16, 16, 0, 0, 24, 16, 1, 128, 8, 32, 1, 128, 4, 32, 1, 128, 4, 32, 1, 128, 4, 0, 1, 128, 6, 64, 1, 128, 2, 64, 1, 128, 2, 64, 1, 128, 2, 64, 1, 128, 2, 96, 0, 0, 0, 32, 0, 0, 4, 32, 0, 0, 4, 32, 1, 128, 4, 16, 1, 128, 8, 24, 0, 0, 8, 8, 0, 0, 16, 4, 0, 0, 32, 2, 0, 0, 64, 1, 0, 1, 128, 0, 192, 3, 0, 0, 56, 60, 0, 0, 3, 224, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 15, 224, 0, 0, 56, 28, 0, 0, 192, 3, 0, 1, 0, 0, 128, 2, 0, 0, 64, 4, 0, 0, 32, 8, 0, 0, 16, 16, 0, 0, 8, 16, 1, 128, 8, 32, 1, 128, 4, 32, 1, 128, 4, 32, 1, 128, 6, 64, 1, 128, 2, 64, 1, 128, 2, 64, 1, 128, 2, 64, 1, 128, 2, 64, 1, 128, 2, 64, 0, 0, 2, 96, 0, 0, 4, 32, 0, 0, 4, 32, 1, 128, 4, 16, 1, 128, 8, 16, 0, 0, 8, 8, 0, 0, 16, 4, 0, 0, 32, 2, 0, 0, 64, 1, 0, 0, 128, 0, 192, 3, 0, 0, 56, 28, 0, 0, 7, 240, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 7, 224, 0, 0, 32, 4, 0, 0, 128, 1, 0, 0, 0, 0, 128, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 16, 0, 0, 8, 0, 1, 128, 0, 32, 1, 128, 4, 0, 1, 128, 0, 0, 1, 128, 0, 64, 1, 128, 2, 64, 1, 128, 2, 64, 1, 128, 2, 64, 1, 128, 2, 64, 1, 128, 2, 64, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 32, 1, 128, 4, 0, 1, 128, 0, 16, 0, 0, 8, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 128, 1, 0, 0, 32, 4, 0, 0, 7, 224, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 224, 0, 0, 32, 4, 0, 0, 128, 1, 0, 1, 0, 0, 128, 2, 0, 0, 64, 4, 0, 0, 32, 0, 1, 128, 0, 8, 1, 128, 16, 0, 1, 128, 0, 0, 1, 128, 0, 16, 1, 128, 8, 16, 1, 128, 8, 16, 1, 128, 0, 0, 1, 128, 8, 16, 1, 128, 8, 16, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 8, 1, 128, 16, 0, 1, 128, 0, 4, 0, 0, 32, 2, 0, 0, 64, 1, 0, 0, 128, 0, 128, 1, 0, 0, 32, 4, 0, 0, 7, 96, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 24, 24, 0, 0, 0, 6, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 8, 1, 128, 0, 8, 1, 128, 0, 16, 1, 128, 8, 16, 1, 128, 8, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 1, 128, 0, 0, 0, 0, 0, 16, 0, 0, 8, 16, 0, 0, 8, 0, 1, 128, 16, 0, 1, 128, 16, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 128, 0, 0, 0, 96, 0, 0, 0, 24, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 192, 0, 0, 60, 24, 0, 0, 224, 2, 0, 1, 128, 0, 128, 2, 0, 0, 64, 4, 0, 0, 32, 0, 0, 0, 48, 8, 1, 128, 16, 0, 1, 128, 24, 16, 1, 128, 8, 16, 1, 128, 8, 0, 1, 128, 12, 32, 1, 128, 4, 32, 1, 128, 4, 32, 1, 128, 4, 32, 1, 128, 4, 48, 0, 0, 0, 16, 0, 0, 8, 16, 0, 0, 8, 24, 1, 128, 0, 8, 1, 128, 16, 12, 0, 0, 0, 4, 0, 0, 32, 2, 0, 0, 64, 1, 0, 1, 128, 0, 64, 7, 0, 0, 24, 60, 0, 0, 3, 224, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 240, 0, 0, 48, 12, 0, 0, 192, 3, 0, 1, 0, 0, 128, 2, 0, 0, 64, 4, 0, 0, 32, 8, 0, 0, 16, 8, 1, 128, 16, 16, 1, 128, 8, 16, 1, 128, 8, 32, 1, 128, 4, 32, 1, 128, 4, 32, 1, 128, 4, 32, 1, 128, 4, 32, 1, 128, 4, 32, 1, 128, 4, 32, 0, 0, 4, 32, 0, 0, 4, 16, 0, 0, 8, 16, 1, 128, 8, 8, 1, 128, 16, 8, 0, 0, 16, 4, 0, 0, 32, 2, 0, 0, 64, 1, 0, 0, 128, 0, 192, 3, 0, 0, 48, 12, 0, 0, 15, 240, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 240, 0, 0, 112, 4, 0, 0, 192, 1, 0, 1, 0, 0, 128, 2, 0, 0, 64, 4, 0, 0, 32, 8, 0, 0, 16, 0, 1, 128, 24, 16, 1, 128, 8, 0, 1, 128, 12, 32, 1, 128, 4, 32, 1, 128, 4, 32, 1, 128, 4, 32, 1, 128, 4, 32, 1, 128, 4, 32, 1, 128, 4, 32, 0, 0, 4, 32, 0, 0, 4, 48, 0, 0, 0, 16, 1, 128, 8, 24, 1, 128, 0, 8, 0, 0, 16, 4, 0, 0, 32, 2, 0, 0, 64, 1, 0, 0, 128, 0, 128, 3, 0, 0, 32, 14, 0, 0, 15, 248, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 248, 0, 0, 96, 6, 0, 1, 128, 1, 128, 3, 0, 0, 192, 6, 0, 0, 96, 12, 0, 0, 48, 8, 0, 0, 16, 16, 1, 128, 8, 16, 1, 128, 8, 32, 1, 128, 4, 32, 1, 128, 4, 32, 1, 128, 4, 32, 1, 128, 4, 32, 1, 128, 4, 32, 1, 128, 4, 32, 1, 128, 4, 32, 0, 0, 4, 32, 0, 0, 4, 32, 0, 0, 4, 16, 1, 128, 8, 16, 1, 128, 8, 8, 0, 0, 16, 12, 0, 0, 48, 6, 0, 0, 96, 3, 0, 0, 192, 1, 128, 1, 128, 0, 96, 6, 0, 0, 31, 248, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 1, 0, 0, 0, 31, 248, 0, 0, 96, 6, 0, 1, 128, 1, 128, 3, 0, 0, 192, 6, 0, 0, 96, 12, 0, 0, 48, 8, 0, 0, 16, 16, 1, 128, 8, 16, 1, 128, 8, 32, 1, 128, 4, 32, 1, 128, 4, 32, 1, 128, 4, 32, 1, 128, 4, 32, 1, 128, 6, 96, 1, 128, 4, 32, 1, 128, 4, 32, 0, 0, 4, 32, 0, 0, 4, 32, 0, 0, 4, 16, 1, 128, 8, 16, 1, 128, 8, 8, 0, 0, 16, 12, 0, 0, 48, 6, 0, 0, 96, 3, 0, 0, 192, 1, 128, 1, 128, 0, 96, 6, 0, 0, 31, 248, 0, 0, 0, 128, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 1, 0, 0, 0, 31, 120, 0, 0, 96, 6, 0, 1, 128, 1, 128, 2, 0, 0, 64, 4, 0, 0, 32, 8, 0, 0, 16, 8, 0, 0, 16, 16, 1, 128, 8, 16, 1, 128, 8, 32, 1, 128, 4, 32, 1, 128, 4, 32, 1, 128, 4, 32, 1, 128, 4, 0, 1, 128, 6, 96, 1, 128, 0, 32, 1, 128, 4, 32, 0, 0, 4, 32, 0, 0, 4, 32, 0, 0, 4, 16, 1, 128, 8, 16, 1, 128, 8, 8, 0, 0, 16, 8, 0, 0, 16, 4, 0, 0, 32, 2, 0, 0, 64, 1, 128, 1, 128, 0, 96, 6, 0, 0, 30, 248, 0, 0, 0, 128, 0, 0, 0, 0, 0 }
  };
  if (pos == 0) {
    display.fillRoundRect(UI_MIDDLE_X - 2 - 42, UI_MIDDLE_Y - 2, 36, 36, 10, 0);
    display.drawBitmap(UI_MIDDLE_X - 42, UI_MIDDLE_Y, frames[0], FRAME_WIDTH, FRAME_HEIGHT, 1);
  }
  if (pos == 1) {
    for (int frame = 0; frame < 28; frame++) {
      display.fillRoundRect(UI_MIDDLE_X - 2, UI_MIDDLE_Y - 2, 36, 36, 10, 0);
      display.drawBitmap(UI_MIDDLE_X, UI_MIDDLE_Y, frames[frame], FRAME_WIDTH, FRAME_HEIGHT, 1);
      display.display();
    }
  }
  if (pos == 2) {
    display.fillRoundRect(UI_MIDDLE_X - 2 + 42, UI_MIDDLE_Y - 2, 36, 36, 10, 0);
    display.drawBitmap(UI_MIDDLE_X + 42, UI_MIDDLE_Y, frames[0], FRAME_WIDTH, FRAME_HEIGHT, 1);
  }
}

void UI_pause(int pos) {
  const byte PROGMEM frames[][128] = {
    { 0, 0, 0, 0, 0, 3, 224, 0, 0, 63, 252, 0, 0, 224, 15, 0, 1, 128, 15, 128, 3, 0, 8, 192, 6, 0, 24, 96, 12, 0, 24, 48, 24, 0, 48, 24, 24, 0, 48, 8, 48, 1, 224, 28, 48, 15, 240, 124, 32, 28, 59, 196, 96, 24, 31, 4, 96, 49, 140, 6, 96, 51, 204, 6, 96, 51, 204, 6, 96, 49, 140, 6, 96, 248, 24, 6, 35, 220, 56, 4, 62, 15, 240, 12, 56, 7, 128, 12, 16, 12, 0, 24, 24, 12, 0, 24, 12, 24, 0, 48, 6, 24, 0, 96, 7, 16, 0, 192, 1, 240, 1, 128, 0, 240, 7, 0, 0, 63, 252, 0, 0, 7, 192, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 3, 224, 0, 0, 63, 252, 0, 0, 224, 15, 0, 1, 128, 15, 128, 3, 0, 8, 192, 6, 0, 24, 96, 12, 0, 24, 48, 24, 0, 48, 24, 24, 0, 48, 8, 48, 1, 224, 28, 48, 15, 240, 124, 32, 28, 59, 196, 96, 24, 31, 4, 96, 49, 140, 6, 96, 51, 204, 6, 96, 51, 204, 6, 96, 49, 140, 6, 32, 248, 24, 6, 35, 220, 56, 4, 62, 15, 240, 12, 56, 7, 128, 12, 16, 12, 0, 24, 24, 12, 0, 24, 12, 24, 0, 48, 6, 24, 0, 96, 3, 16, 0, 192, 1, 240, 1, 128, 0, 240, 7, 0, 0, 63, 252, 0, 0, 7, 192, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 3, 224, 0, 0, 63, 252, 0, 0, 224, 15, 0, 1, 128, 15, 128, 3, 0, 8, 192, 6, 0, 24, 96, 12, 0, 24, 48, 24, 0, 48, 24, 24, 0, 48, 8, 48, 1, 224, 28, 48, 15, 240, 124, 32, 28, 59, 196, 96, 24, 31, 4, 96, 49, 140, 6, 96, 51, 204, 6, 96, 51, 204, 6, 0, 49, 140, 6, 32, 248, 24, 6, 35, 220, 56, 4, 62, 15, 240, 12, 56, 7, 128, 12, 16, 12, 0, 24, 24, 12, 0, 24, 12, 24, 0, 48, 6, 24, 0, 96, 3, 16, 0, 192, 1, 240, 1, 128, 0, 240, 7, 0, 0, 63, 252, 0, 0, 3, 192, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 3, 224, 0, 0, 63, 252, 0, 0, 224, 15, 0, 1, 192, 15, 128, 3, 0, 8, 192, 6, 0, 24, 96, 12, 0, 24, 48, 24, 0, 48, 24, 24, 0, 48, 8, 48, 1, 224, 28, 48, 15, 240, 124, 32, 28, 59, 196, 32, 24, 31, 4, 0, 49, 140, 6, 0, 51, 204, 6, 64, 51, 204, 6, 96, 49, 140, 6, 32, 248, 24, 6, 35, 220, 56, 4, 62, 15, 240, 12, 56, 7, 128, 12, 16, 12, 0, 24, 24, 12, 0, 24, 12, 24, 0, 48, 6, 24, 0, 96, 3, 16, 0, 192, 1, 240, 1, 128, 0, 240, 7, 0, 0, 63, 252, 0, 0, 7, 192, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 3, 224, 0, 0, 63, 252, 0, 0, 224, 15, 0, 1, 128, 15, 128, 3, 0, 8, 192, 14, 0, 24, 96, 12, 0, 24, 48, 24, 0, 48, 24, 24, 0, 48, 8, 16, 1, 224, 28, 0, 15, 240, 124, 0, 28, 59, 196, 0, 24, 31, 4, 4, 49, 140, 6, 101, 51, 204, 6, 96, 51, 204, 6, 96, 49, 140, 6, 32, 248, 24, 6, 35, 220, 56, 4, 62, 15, 240, 12, 56, 7, 128, 12, 16, 12, 0, 24, 24, 12, 0, 24, 12, 24, 0, 48, 6, 24, 0, 96, 3, 16, 0, 192, 1, 240, 1, 128, 0, 240, 7, 0, 0, 63, 252, 0, 0, 7, 192, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 3, 224, 0, 0, 63, 252, 0, 0, 224, 15, 0, 1, 128, 15, 128, 3, 0, 8, 192, 6, 0, 24, 96, 4, 0, 24, 48, 0, 0, 48, 24, 0, 0, 48, 8, 0, 1, 224, 28, 4, 15, 240, 124, 38, 156, 59, 196, 101, 152, 31, 4, 97, 177, 140, 6, 96, 51, 204, 6, 96, 51, 204, 6, 96, 49, 140, 6, 32, 248, 24, 6, 35, 220, 56, 4, 62, 15, 240, 12, 56, 7, 128, 12, 16, 12, 0, 24, 24, 12, 0, 24, 12, 24, 0, 48, 6, 24, 0, 96, 3, 16, 0, 192, 1, 240, 1, 128, 0, 240, 7, 0, 0, 63, 252, 0, 0, 7, 192, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 3, 224, 0, 0, 63, 252, 0, 0, 224, 15, 0, 1, 192, 15, 128, 1, 0, 8, 192, 0, 0, 24, 96, 0, 0, 24, 48, 1, 0, 48, 24, 3, 0, 48, 8, 3, 65, 224, 28, 38, 207, 240, 124, 32, 220, 59, 196, 32, 24, 31, 4, 96, 49, 140, 6, 96, 51, 204, 6, 96, 51, 204, 6, 96, 49, 140, 6, 32, 248, 24, 6, 35, 220, 56, 4, 62, 15, 240, 12, 56, 7, 128, 12, 16, 12, 0, 24, 24, 12, 0, 24, 12, 24, 0, 48, 6, 24, 0, 96, 3, 16, 0, 192, 1, 240, 1, 128, 0, 240, 7, 0, 0, 63, 252, 0, 0, 7, 192, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 7, 224, 0, 0, 63, 252, 0, 0, 96, 15, 0, 0, 0, 15, 128, 0, 0, 8, 192, 0, 32, 24, 96, 0, 224, 24, 48, 1, 144, 48, 24, 19, 48, 48, 8, 51, 97, 224, 28, 48, 207, 240, 124, 32, 28, 59, 196, 96, 24, 31, 4, 96, 49, 140, 6, 96, 51, 204, 6, 96, 51, 204, 6, 96, 49, 140, 6, 32, 248, 24, 6, 35, 220, 56, 4, 62, 15, 240, 12, 56, 7, 128, 12, 16, 12, 0, 24, 24, 12, 0, 24, 12, 24, 0, 48, 6, 24, 0, 96, 3, 16, 0, 192, 1, 240, 1, 128, 0, 240, 7, 0, 0, 63, 252, 0, 0, 7, 192, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 3, 224, 0, 0, 31, 252, 0, 0, 0, 15, 0, 0, 0, 15, 128, 0, 0, 8, 192, 4, 112, 24, 96, 12, 224, 24, 48, 25, 152, 48, 24, 27, 48, 48, 8, 48, 97, 224, 28, 48, 79, 240, 124, 32, 28, 59, 196, 96, 24, 31, 4, 96, 49, 140, 6, 96, 51, 204, 6, 96, 51, 204, 6, 96, 49, 140, 6, 32, 248, 24, 6, 35, 220, 56, 4, 62, 15, 240, 12, 56, 7, 128, 12, 16, 12, 0, 24, 24, 12, 0, 24, 12, 24, 0, 48, 6, 24, 0, 96, 3, 16, 0, 192, 1, 240, 1, 128, 0, 240, 7, 0, 0, 63, 252, 0, 0, 7, 192, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 3, 224, 0, 0, 7, 252, 0, 0, 0, 15, 0, 1, 128, 15, 128, 3, 24, 8, 192, 6, 48, 24, 96, 12, 224, 24, 48, 24, 28, 48, 24, 24, 48, 48, 8, 48, 1, 224, 28, 48, 15, 240, 124, 32, 28, 59, 196, 32, 24, 31, 4, 96, 49, 140, 6, 96, 51, 204, 6, 96, 51, 204, 6, 96, 49, 140, 6, 32, 248, 24, 6, 35, 220, 56, 4, 62, 15, 240, 12, 56, 7, 128, 12, 16, 12, 0, 24, 24, 12, 0, 24, 12, 24, 0, 48, 6, 24, 0, 96, 3, 16, 0, 192, 1, 240, 1, 128, 0, 240, 7, 0, 0, 63, 252, 0, 0, 7, 192, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 1, 224, 0, 0, 49, 252, 0, 0, 224, 15, 0, 1, 128, 15, 128, 3, 6, 8, 192, 6, 0, 24, 96, 12, 6, 24, 48, 24, 2, 48, 24, 24, 0, 48, 8, 48, 1, 224, 28, 48, 15, 240, 124, 32, 28, 59, 196, 96, 24, 31, 4, 96, 49, 140, 6, 96, 51, 204, 6, 96, 51, 204, 6, 96, 49, 140, 6, 32, 248, 24, 6, 35, 220, 56, 4, 62, 15, 240, 12, 56, 7, 128, 12, 16, 12, 0, 24, 24, 12, 0, 24, 12, 24, 0, 48, 6, 24, 0, 96, 3, 16, 0, 192, 1, 240, 1, 128, 0, 240, 7, 0, 0, 63, 252, 0, 0, 7, 192, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 2, 32, 0, 0, 62, 124, 0, 0, 224, 15, 0, 1, 128, 15, 128, 3, 0, 8, 192, 6, 0, 24, 96, 12, 0, 24, 48, 24, 0, 48, 24, 24, 0, 48, 8, 48, 1, 224, 28, 48, 15, 240, 124, 32, 28, 59, 196, 96, 24, 31, 4, 96, 49, 140, 6, 96, 51, 204, 6, 96, 51, 204, 6, 96, 49, 140, 6, 32, 248, 24, 6, 35, 220, 56, 4, 62, 15, 240, 12, 56, 7, 128, 12, 16, 12, 0, 24, 24, 12, 0, 24, 12, 24, 0, 48, 6, 24, 0, 96, 3, 16, 0, 192, 1, 240, 1, 128, 0, 240, 7, 0, 0, 63, 252, 0, 0, 7, 192, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 3, 224, 0, 0, 63, 252, 0, 0, 224, 15, 0, 1, 128, 15, 128, 3, 0, 8, 192, 6, 0, 24, 96, 12, 0, 24, 48, 24, 0, 48, 24, 24, 0, 48, 24, 48, 1, 224, 28, 48, 15, 240, 124, 32, 28, 59, 196, 96, 24, 31, 4, 96, 49, 140, 6, 96, 51, 204, 6, 96, 51, 204, 6, 96, 49, 140, 6, 32, 248, 24, 6, 35, 220, 56, 4, 62, 15, 240, 12, 56, 7, 128, 12, 16, 12, 0, 24, 24, 12, 0, 24, 12, 24, 0, 48, 6, 24, 0, 96, 3, 16, 0, 192, 1, 240, 1, 128, 0, 240, 7, 0, 0, 63, 252, 0, 0, 7, 192, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 3, 240, 0, 0, 63, 252, 0, 0, 224, 15, 0, 1, 128, 15, 128, 3, 0, 8, 192, 6, 0, 24, 96, 12, 0, 24, 48, 24, 0, 48, 24, 24, 0, 48, 8, 48, 1, 224, 28, 48, 15, 240, 124, 32, 28, 59, 196, 96, 24, 31, 4, 96, 49, 140, 6, 96, 51, 204, 6, 96, 51, 204, 6, 96, 49, 140, 6, 32, 248, 24, 6, 35, 220, 56, 4, 62, 15, 240, 12, 56, 7, 128, 12, 16, 12, 0, 24, 24, 12, 0, 24, 12, 24, 0, 48, 6, 24, 0, 96, 3, 16, 0, 192, 1, 240, 1, 128, 0, 240, 7, 0, 0, 63, 252, 0, 0, 7, 192, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 3, 224, 0, 0, 63, 252, 0, 0, 224, 15, 0, 1, 128, 15, 128, 3, 0, 8, 192, 6, 0, 24, 96, 12, 0, 24, 48, 24, 0, 48, 24, 24, 0, 48, 8, 48, 1, 224, 28, 48, 15, 240, 124, 32, 28, 59, 196, 96, 24, 31, 6, 96, 49, 140, 6, 96, 51, 204, 6, 96, 51, 204, 6, 96, 49, 140, 6, 32, 248, 24, 6, 35, 220, 56, 4, 62, 15, 240, 12, 56, 7, 128, 12, 16, 12, 0, 24, 24, 12, 0, 24, 12, 24, 0, 48, 6, 24, 0, 96, 3, 16, 0, 192, 1, 240, 1, 128, 0, 240, 7, 0, 0, 63, 252, 0, 0, 7, 192, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 3, 224, 0, 0, 63, 252, 0, 0, 224, 15, 0, 1, 128, 15, 128, 3, 0, 8, 192, 6, 0, 24, 96, 12, 0, 24, 48, 24, 0, 48, 24, 24, 0, 48, 8, 48, 1, 224, 28, 48, 15, 240, 124, 32, 28, 59, 196, 96, 24, 31, 6, 96, 49, 140, 6, 96, 51, 204, 6, 96, 51, 204, 6, 96, 49, 140, 6, 32, 248, 24, 6, 35, 220, 56, 4, 62, 15, 240, 12, 56, 7, 128, 12, 16, 12, 0, 24, 24, 12, 0, 24, 12, 24, 0, 48, 6, 24, 0, 96, 3, 16, 0, 192, 1, 240, 1, 128, 0, 240, 7, 0, 0, 63, 252, 0, 0, 7, 192, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 3, 192, 0, 0, 63, 252, 0, 0, 224, 15, 0, 1, 128, 15, 128, 3, 0, 8, 192, 6, 0, 24, 96, 12, 0, 24, 48, 24, 0, 48, 24, 24, 0, 48, 8, 48, 1, 224, 28, 48, 15, 240, 124, 32, 28, 59, 196, 96, 24, 31, 0, 96, 49, 140, 6, 96, 51, 204, 6, 96, 51, 204, 6, 96, 49, 140, 6, 32, 248, 24, 6, 35, 220, 56, 4, 62, 15, 240, 12, 56, 7, 128, 12, 16, 12, 0, 24, 24, 12, 0, 24, 12, 24, 0, 48, 6, 24, 0, 96, 3, 16, 0, 192, 1, 240, 1, 128, 0, 240, 7, 0, 0, 63, 252, 0, 0, 7, 192, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 3, 224, 0, 0, 63, 252, 0, 0, 224, 15, 0, 1, 128, 15, 128, 3, 0, 8, 192, 6, 0, 24, 96, 12, 0, 24, 48, 24, 0, 48, 24, 24, 0, 48, 8, 48, 1, 224, 28, 48, 15, 240, 124, 32, 28, 59, 196, 96, 24, 31, 4, 96, 49, 140, 6, 96, 51, 204, 0, 96, 51, 204, 0, 96, 49, 140, 0, 32, 248, 24, 4, 35, 220, 56, 4, 62, 15, 240, 12, 56, 7, 128, 12, 16, 12, 0, 24, 24, 12, 0, 24, 12, 24, 0, 48, 6, 24, 0, 96, 3, 16, 0, 192, 1, 240, 3, 128, 0, 240, 7, 0, 0, 63, 252, 0, 0, 7, 192, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 3, 224, 0, 0, 63, 252, 0, 0, 224, 15, 0, 1, 128, 15, 128, 3, 0, 8, 192, 6, 0, 24, 96, 12, 0, 24, 48, 24, 0, 48, 24, 24, 0, 48, 8, 48, 1, 224, 28, 48, 15, 240, 124, 32, 28, 59, 196, 96, 24, 31, 4, 96, 49, 140, 6, 96, 51, 204, 6, 96, 51, 204, 166, 96, 49, 140, 32, 32, 248, 24, 0, 35, 220, 56, 0, 62, 15, 240, 0, 56, 7, 128, 0, 16, 12, 0, 24, 24, 12, 0, 24, 12, 24, 0, 48, 6, 24, 0, 96, 3, 16, 0, 192, 1, 240, 1, 128, 0, 240, 7, 0, 0, 63, 252, 0, 0, 7, 192, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 3, 224, 0, 0, 63, 252, 0, 0, 224, 15, 0, 1, 128, 15, 128, 3, 0, 8, 192, 6, 0, 24, 96, 12, 0, 24, 48, 24, 0, 48, 24, 24, 0, 48, 8, 48, 1, 224, 28, 48, 15, 240, 124, 32, 28, 59, 196, 96, 24, 31, 4, 96, 49, 140, 6, 96, 51, 204, 6, 96, 51, 204, 6, 96, 49, 141, 166, 32, 248, 25, 166, 35, 220, 57, 100, 62, 15, 240, 0, 56, 7, 128, 0, 16, 12, 0, 0, 24, 12, 0, 0, 12, 24, 0, 0, 6, 24, 0, 96, 3, 16, 0, 192, 1, 240, 1, 128, 0, 240, 7, 0, 0, 63, 252, 0, 0, 7, 192, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 3, 224, 0, 0, 63, 252, 0, 0, 224, 15, 0, 1, 128, 15, 128, 3, 0, 8, 192, 6, 0, 24, 96, 12, 0, 24, 48, 24, 0, 48, 24, 24, 0, 48, 8, 48, 1, 224, 28, 48, 15, 240, 124, 32, 28, 59, 196, 96, 24, 31, 4, 96, 49, 140, 6, 96, 51, 204, 6, 96, 51, 204, 6, 96, 49, 140, 6, 32, 248, 25, 6, 35, 220, 59, 100, 62, 15, 243, 108, 56, 7, 130, 196, 16, 12, 0, 192, 24, 12, 0, 0, 12, 24, 0, 0, 6, 24, 0, 0, 3, 16, 0, 0, 1, 240, 1, 0, 0, 240, 7, 0, 0, 63, 252, 0, 0, 7, 192, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 3, 224, 0, 0, 63, 252, 0, 0, 224, 15, 0, 1, 128, 15, 128, 3, 0, 8, 192, 6, 0, 24, 96, 12, 0, 24, 48, 24, 0, 48, 24, 24, 0, 48, 8, 48, 1, 224, 28, 48, 15, 240, 124, 32, 28, 59, 196, 96, 24, 31, 4, 96, 49, 140, 6, 96, 51, 204, 6, 96, 51, 204, 6, 96, 49, 140, 6, 32, 248, 24, 4, 35, 220, 59, 4, 62, 15, 243, 76, 56, 7, 134, 204, 16, 12, 4, 216, 24, 12, 1, 152, 12, 24, 3, 48, 6, 24, 0, 32, 3, 16, 0, 0, 1, 240, 0, 0, 0, 240, 0, 0, 0, 63, 252, 0, 0, 7, 224, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 3, 224, 0, 0, 63, 252, 0, 0, 224, 15, 0, 1, 128, 15, 128, 3, 0, 8, 192, 6, 0, 24, 96, 12, 0, 24, 48, 24, 0, 48, 24, 24, 0, 48, 8, 48, 1, 224, 28, 48, 15, 240, 124, 32, 28, 59, 196, 96, 24, 31, 4, 96, 49, 140, 6, 96, 51, 204, 6, 96, 51, 204, 6, 96, 49, 140, 6, 32, 248, 24, 4, 35, 220, 57, 4, 62, 15, 243, 76, 56, 7, 134, 204, 16, 12, 12, 216, 24, 12, 9, 152, 12, 24, 7, 48, 6, 24, 6, 96, 3, 16, 0, 192, 1, 240, 1, 128, 0, 240, 1, 0, 0, 63, 224, 0, 0, 7, 192, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 3, 224, 0, 0, 63, 252, 0, 0, 224, 15, 0, 1, 128, 15, 128, 3, 0, 8, 192, 6, 0, 24, 96, 12, 0, 24, 48, 24, 0, 48, 24, 24, 0, 48, 8, 48, 1, 224, 28, 48, 15, 240, 124, 32, 28, 59, 196, 96, 24, 31, 4, 96, 49, 140, 6, 96, 51, 204, 6, 96, 51, 204, 6, 96, 49, 140, 6, 32, 248, 24, 4, 35, 220, 56, 4, 62, 15, 242, 12, 56, 7, 134, 76, 16, 12, 12, 216, 24, 12, 9, 152, 12, 24, 7, 48, 6, 24, 6, 96, 3, 16, 0, 192, 1, 240, 1, 128, 0, 240, 15, 0, 0, 63, 24, 0, 0, 7, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 3, 224, 0, 0, 63, 252, 0, 0, 224, 15, 0, 1, 128, 15, 128, 3, 0, 8, 192, 6, 0, 24, 96, 12, 0, 24, 48, 24, 0, 48, 24, 24, 0, 48, 8, 48, 1, 224, 28, 48, 15, 240, 124, 32, 28, 59, 196, 96, 24, 31, 4, 96, 49, 140, 6, 96, 51, 204, 6, 96, 51, 204, 6, 96, 49, 140, 6, 32, 248, 24, 6, 35, 220, 56, 4, 62, 15, 240, 12, 56, 7, 128, 12, 24, 12, 12, 24, 24, 12, 56, 24, 12, 24, 7, 48, 6, 24, 12, 96, 3, 16, 24, 192, 1, 240, 1, 128, 0, 240, 7, 0, 0, 57, 252, 0, 0, 3, 192, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 3, 224, 0, 0, 63, 252, 0, 0, 224, 15, 0, 1, 128, 15, 128, 3, 0, 8, 192, 6, 0, 24, 96, 12, 0, 24, 48, 24, 0, 48, 24, 24, 0, 48, 8, 48, 1, 224, 28, 48, 15, 240, 124, 32, 28, 59, 196, 96, 24, 31, 4, 96, 49, 140, 6, 96, 51, 204, 6, 96, 51, 204, 6, 96, 49, 140, 6, 32, 248, 24, 6, 35, 220, 56, 4, 62, 15, 240, 12, 56, 7, 128, 12, 16, 12, 0, 24, 24, 12, 32, 24, 12, 24, 96, 48, 6, 24, 0, 96, 3, 16, 120, 192, 1, 240, 1, 128, 0, 240, 7, 0, 0, 63, 252, 0, 0, 15, 192, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 3, 224, 0, 0, 63, 252, 0, 0, 224, 15, 0, 1, 128, 15, 128, 3, 0, 8, 192, 6, 0, 24, 96, 12, 0, 24, 48, 24, 0, 48, 24, 24, 0, 48, 8, 48, 1, 224, 28, 48, 15, 240, 124, 32, 28, 59, 196, 96, 24, 31, 4, 96, 49, 140, 6, 96, 51, 204, 6, 96, 51, 204, 6, 96, 49, 140, 6, 32, 248, 24, 6, 35, 220, 56, 4, 62, 15, 240, 12, 56, 7, 128, 12, 16, 12, 0, 24, 24, 12, 0, 24, 12, 24, 192, 48, 6, 24, 0, 96, 3, 16, 192, 192, 1, 240, 129, 128, 0, 240, 7, 0, 0, 63, 252, 0, 0, 15, 192, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 3, 224, 0, 0, 63, 252, 0, 0, 224, 15, 0, 1, 128, 15, 128, 3, 0, 8, 192, 6, 0, 24, 96, 12, 0, 24, 48, 24, 0, 48, 24, 24, 0, 48, 8, 48, 1, 224, 28, 48, 15, 240, 124, 32, 28, 59, 196, 96, 24, 31, 4, 96, 49, 140, 6, 96, 51, 204, 6, 96, 51, 204, 6, 96, 49, 140, 6, 32, 248, 24, 6, 35, 220, 56, 4, 62, 15, 240, 12, 56, 7, 128, 12, 16, 12, 0, 24, 24, 12, 0, 24, 12, 24, 0, 48, 6, 24, 0, 96, 3, 16, 0, 192, 1, 240, 1, 128, 0, 240, 7, 0, 0, 63, 252, 0, 0, 15, 192, 0, 0, 0, 0, 0 }
  };
  if (pos == 0) {
    display.fillRoundRect(UI_MIDDLE_X - 2 - 42, UI_MIDDLE_Y - 2, 36, 36, 10, 0);
    display.drawBitmap(UI_MIDDLE_X - 42, UI_MIDDLE_Y, frames[0], FRAME_WIDTH, FRAME_HEIGHT, 1);
  }
  if (pos == 1) {
    for (int frame = 0; frame < 28; frame++) {
      display.fillRoundRect(UI_MIDDLE_X - 2, UI_MIDDLE_Y - 2, 36, 36, 10, 0);
      display.drawBitmap(UI_MIDDLE_X, UI_MIDDLE_Y, frames[frame], FRAME_WIDTH, FRAME_HEIGHT, 1);
      display.display();
    }
  }
  if (pos == 2) {
    display.fillRoundRect(UI_MIDDLE_X - 2 + 42, UI_MIDDLE_Y - 2, 36, 36, 10, 0);
    display.drawBitmap(UI_MIDDLE_X + 42, UI_MIDDLE_Y, frames[0], FRAME_WIDTH, FRAME_HEIGHT, 1);
  }
}

void UI_resume(int pos) {
  const byte PROGMEM frames[][128] = {
    { 0, 0, 0, 0, 0, 3, 224, 0, 0, 63, 252, 0, 0, 224, 15, 0, 1, 128, 3, 128, 3, 0, 0, 192, 6, 0, 0, 96, 12, 0, 0, 48, 24, 16, 0, 24, 24, 28, 0, 8, 48, 31, 0, 12, 48, 19, 192, 4, 32, 16, 224, 4, 96, 16, 56, 4, 96, 16, 30, 6, 96, 16, 7, 6, 96, 16, 7, 6, 96, 16, 14, 6, 32, 16, 56, 6, 32, 16, 224, 4, 32, 17, 128, 12, 48, 31, 0, 12, 16, 28, 0, 24, 24, 16, 0, 24, 12, 0, 0, 48, 6, 0, 0, 96, 3, 0, 0, 192, 1, 192, 1, 128, 0, 240, 7, 0, 0, 63, 252, 0, 0, 7, 192, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 3, 224, 0, 0, 63, 252, 0, 0, 224, 15, 0, 1, 128, 3, 128, 3, 0, 0, 192, 6, 0, 0, 96, 12, 0, 0, 48, 24, 16, 0, 24, 24, 28, 0, 8, 48, 31, 0, 12, 48, 19, 192, 4, 32, 16, 224, 4, 96, 16, 56, 4, 96, 16, 30, 6, 96, 16, 7, 6, 96, 16, 7, 6, 96, 16, 14, 6, 32, 16, 56, 6, 32, 16, 224, 4, 32, 17, 128, 12, 48, 31, 0, 12, 16, 28, 0, 24, 24, 16, 0, 24, 12, 0, 0, 48, 6, 0, 0, 96, 3, 0, 0, 192, 1, 192, 1, 128, 0, 240, 7, 0, 0, 63, 252, 0, 0, 7, 192, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 3, 224, 0, 0, 63, 252, 0, 0, 224, 15, 0, 1, 128, 3, 128, 3, 0, 0, 192, 6, 0, 0, 96, 12, 0, 0, 48, 24, 16, 0, 24, 24, 28, 0, 8, 48, 31, 0, 12, 48, 19, 192, 4, 32, 16, 224, 4, 96, 16, 56, 4, 96, 16, 30, 6, 96, 16, 7, 6, 96, 16, 7, 6, 96, 16, 14, 6, 32, 16, 56, 6, 32, 16, 224, 4, 32, 17, 128, 12, 48, 31, 0, 12, 16, 28, 0, 24, 24, 16, 0, 24, 12, 0, 0, 48, 6, 0, 0, 96, 3, 0, 0, 192, 1, 192, 1, 128, 0, 240, 7, 0, 0, 63, 252, 0, 0, 7, 192, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 3, 224, 0, 0, 63, 252, 0, 0, 224, 15, 0, 1, 128, 3, 128, 3, 0, 0, 192, 6, 0, 0, 96, 12, 0, 0, 48, 24, 16, 0, 24, 24, 28, 0, 8, 48, 31, 0, 12, 48, 19, 192, 4, 32, 16, 224, 4, 96, 16, 56, 4, 96, 16, 30, 6, 96, 16, 7, 6, 96, 16, 7, 6, 96, 16, 14, 6, 32, 16, 56, 6, 32, 16, 224, 4, 32, 17, 128, 12, 48, 31, 0, 12, 16, 28, 0, 24, 24, 16, 0, 24, 12, 0, 0, 48, 6, 0, 0, 96, 3, 0, 0, 192, 1, 192, 1, 128, 0, 240, 7, 0, 0, 63, 252, 0, 0, 7, 192, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 3, 224, 0, 0, 63, 252, 0, 0, 224, 15, 0, 1, 128, 3, 128, 3, 0, 0, 192, 6, 0, 0, 96, 12, 0, 0, 48, 24, 56, 0, 24, 24, 62, 0, 8, 48, 55, 128, 12, 48, 49, 192, 4, 32, 48, 240, 4, 96, 48, 60, 4, 96, 48, 15, 6, 96, 48, 3, 134, 96, 48, 3, 134, 96, 48, 14, 6, 32, 48, 60, 6, 32, 48, 112, 4, 32, 49, 192, 12, 48, 55, 0, 12, 16, 62, 0, 24, 24, 56, 0, 24, 12, 0, 0, 48, 6, 0, 0, 96, 3, 0, 0, 192, 1, 192, 1, 128, 0, 240, 7, 0, 0, 63, 252, 0, 0, 7, 192, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 3, 224, 0, 0, 63, 252, 0, 0, 224, 15, 0, 1, 128, 3, 128, 3, 0, 0, 192, 6, 0, 0, 96, 12, 112, 0, 48, 24, 124, 0, 24, 24, 111, 0, 8, 48, 103, 128, 12, 48, 97, 224, 4, 32, 96, 120, 4, 96, 96, 30, 4, 96, 96, 15, 6, 96, 96, 3, 134, 96, 96, 3, 198, 96, 96, 7, 6, 32, 96, 28, 6, 32, 96, 120, 4, 32, 97, 224, 12, 48, 99, 128, 12, 16, 110, 0, 24, 24, 124, 0, 24, 12, 48, 0, 48, 6, 0, 0, 96, 3, 0, 0, 192, 1, 192, 1, 128, 0, 240, 7, 0, 0, 63, 252, 0, 0, 7, 192, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 3, 32, 0, 0, 63, 124, 0, 0, 224, 15, 0, 1, 128, 3, 128, 3, 0, 0, 192, 6, 0, 0, 96, 12, 48, 0, 48, 24, 60, 0, 24, 24, 62, 0, 8, 48, 39, 128, 12, 48, 33, 224, 4, 32, 32, 120, 4, 96, 32, 60, 4, 96, 32, 15, 6, 96, 32, 3, 134, 96, 32, 3, 134, 96, 32, 15, 6, 32, 32, 28, 6, 32, 32, 112, 4, 32, 33, 224, 12, 48, 35, 128, 12, 16, 62, 0, 24, 24, 56, 0, 24, 12, 48, 0, 48, 6, 0, 0, 96, 3, 0, 0, 192, 1, 192, 1, 128, 0, 240, 7, 0, 0, 63, 252, 0, 0, 7, 192, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 3, 0, 0, 0, 63, 12, 0, 0, 224, 15, 0, 1, 128, 3, 128, 3, 0, 0, 192, 6, 0, 0, 96, 12, 0, 0, 48, 24, 56, 0, 24, 24, 62, 0, 24, 48, 55, 128, 12, 48, 49, 192, 4, 32, 48, 240, 4, 96, 48, 60, 4, 96, 48, 15, 6, 96, 48, 3, 134, 96, 48, 3, 134, 96, 48, 14, 6, 32, 48, 60, 6, 32, 48, 112, 4, 32, 49, 192, 12, 48, 55, 0, 12, 16, 62, 0, 24, 24, 56, 0, 24, 12, 0, 0, 48, 6, 0, 0, 96, 3, 0, 0, 192, 1, 192, 1, 128, 0, 240, 7, 0, 0, 63, 252, 0, 0, 7, 192, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 3, 0, 0, 0, 63, 0, 0, 0, 224, 1, 0, 1, 128, 1, 128, 3, 0, 0, 192, 6, 0, 0, 96, 12, 0, 0, 48, 24, 24, 0, 24, 24, 28, 0, 24, 48, 31, 0, 12, 48, 19, 192, 4, 32, 16, 240, 4, 96, 16, 56, 4, 96, 16, 30, 6, 96, 16, 7, 6, 96, 16, 3, 6, 96, 16, 14, 6, 32, 16, 56, 6, 32, 16, 224, 4, 32, 17, 192, 12, 48, 23, 0, 12, 16, 28, 0, 24, 24, 16, 0, 24, 12, 0, 0, 48, 6, 0, 0, 96, 3, 0, 0, 192, 1, 192, 1, 128, 0, 240, 7, 0, 0, 63, 252, 0, 0, 7, 192, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 3, 0, 0, 0, 63, 0, 0, 0, 224, 0, 0, 1, 128, 0, 0, 3, 0, 0, 0, 6, 0, 0, 0, 12, 0, 0, 16, 24, 16, 0, 24, 24, 28, 0, 24, 48, 31, 0, 12, 48, 19, 192, 4, 32, 16, 224, 4, 96, 16, 56, 4, 96, 16, 30, 6, 96, 16, 7, 6, 96, 16, 7, 6, 96, 16, 14, 6, 32, 16, 56, 6, 32, 16, 224, 4, 32, 17, 128, 12, 48, 31, 0, 12, 16, 28, 0, 24, 24, 16, 0, 24, 12, 0, 0, 48, 6, 0, 0, 96, 3, 0, 0, 192, 1, 192, 1, 128, 0, 240, 7, 0, 0, 63, 252, 0, 0, 7, 192, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 3, 0, 0, 0, 63, 0, 0, 0, 224, 0, 0, 1, 128, 0, 0, 3, 0, 0, 0, 6, 0, 0, 0, 12, 0, 0, 0, 24, 16, 0, 0, 24, 28, 0, 0, 48, 31, 0, 0, 48, 19, 192, 0, 32, 16, 224, 0, 96, 16, 56, 4, 96, 16, 30, 6, 96, 16, 7, 6, 96, 16, 7, 6, 96, 16, 14, 6, 32, 16, 56, 6, 32, 16, 224, 4, 32, 17, 128, 12, 48, 31, 0, 12, 16, 28, 0, 24, 24, 16, 0, 24, 12, 0, 0, 48, 6, 0, 0, 96, 3, 0, 0, 192, 1, 192, 1, 128, 0, 240, 7, 0, 0, 63, 252, 0, 0, 7, 192, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 3, 0, 0, 0, 63, 0, 0, 0, 224, 0, 0, 1, 128, 0, 0, 3, 0, 0, 0, 6, 0, 0, 0, 12, 0, 0, 0, 24, 16, 0, 0, 24, 28, 0, 0, 48, 31, 0, 0, 48, 19, 192, 0, 32, 16, 224, 0, 96, 16, 56, 0, 96, 16, 30, 0, 96, 16, 7, 0, 96, 16, 7, 0, 96, 16, 14, 0, 32, 16, 56, 0, 32, 16, 224, 0, 32, 17, 128, 0, 48, 31, 0, 0, 16, 28, 0, 24, 24, 16, 0, 24, 12, 0, 0, 48, 6, 0, 0, 96, 3, 0, 0, 192, 1, 192, 1, 128, 0, 240, 7, 0, 0, 63, 252, 0, 0, 7, 192, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 3, 0, 0, 0, 63, 0, 0, 0, 224, 0, 0, 1, 128, 0, 0, 3, 0, 0, 0, 6, 0, 0, 0, 12, 0, 0, 0, 24, 16, 0, 0, 24, 28, 0, 0, 48, 31, 0, 0, 48, 19, 192, 0, 32, 16, 224, 0, 96, 16, 56, 0, 96, 16, 30, 0, 96, 16, 7, 0, 96, 16, 7, 0, 96, 16, 14, 0, 32, 16, 56, 0, 32, 16, 224, 0, 32, 17, 128, 0, 48, 31, 0, 0, 16, 28, 0, 0, 24, 16, 0, 0, 12, 0, 0, 0, 6, 0, 0, 0, 3, 0, 0, 0, 1, 192, 0, 0, 0, 240, 0, 0, 0, 63, 240, 0, 0, 7, 224, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 3, 0, 0, 0, 63, 0, 0, 0, 224, 0, 0, 1, 128, 0, 0, 3, 0, 0, 0, 6, 0, 0, 0, 12, 0, 0, 0, 24, 16, 0, 0, 24, 28, 0, 0, 48, 31, 0, 0, 48, 19, 192, 0, 32, 16, 224, 0, 96, 16, 56, 0, 96, 16, 30, 0, 96, 16, 7, 0, 96, 16, 7, 0, 96, 16, 14, 0, 32, 16, 56, 0, 32, 16, 224, 0, 32, 17, 128, 0, 48, 31, 0, 0, 24, 28, 0, 0, 24, 16, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 3, 0, 0, 0, 63, 0, 0, 0, 224, 0, 0, 1, 128, 0, 0, 3, 0, 0, 0, 6, 0, 0, 0, 12, 0, 0, 0, 0, 16, 0, 0, 0, 28, 0, 0, 0, 31, 0, 0, 0, 19, 192, 0, 0, 16, 224, 0, 0, 16, 56, 0, 0, 16, 30, 0, 0, 16, 7, 0, 0, 16, 7, 0, 0, 16, 14, 0, 0, 16, 56, 0, 0, 16, 224, 0, 0, 17, 128, 0, 0, 31, 0, 0, 0, 28, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0, 0, 28, 0, 0, 0, 31, 0, 0, 0, 19, 192, 0, 0, 16, 224, 0, 0, 16, 56, 0, 0, 16, 30, 0, 0, 16, 7, 0, 0, 16, 7, 0, 0, 16, 14, 0, 0, 16, 56, 0, 0, 16, 224, 0, 0, 17, 128, 0, 0, 31, 0, 0, 0, 28, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 224, 0, 0, 0, 252, 0, 0, 0, 15, 0, 0, 0, 3, 128, 0, 0, 0, 192, 0, 0, 0, 96, 0, 0, 0, 0, 0, 16, 0, 0, 0, 28, 0, 0, 0, 31, 0, 0, 0, 19, 192, 0, 0, 16, 224, 0, 0, 16, 56, 0, 0, 16, 30, 0, 0, 16, 7, 0, 0, 16, 7, 0, 0, 16, 14, 0, 0, 16, 56, 0, 0, 16, 224, 0, 0, 17, 128, 0, 0, 31, 0, 0, 0, 28, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 224, 0, 0, 0, 252, 0, 0, 0, 15, 0, 0, 0, 3, 128, 0, 0, 0, 192, 0, 0, 0, 96, 0, 0, 0, 48, 0, 16, 0, 24, 0, 28, 0, 8, 0, 31, 0, 12, 0, 19, 192, 4, 0, 16, 224, 4, 0, 16, 56, 4, 0, 16, 30, 6, 0, 16, 7, 6, 0, 16, 7, 6, 0, 16, 14, 6, 0, 16, 56, 6, 0, 16, 224, 4, 0, 17, 128, 4, 0, 31, 0, 0, 0, 28, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 224, 0, 0, 0, 252, 0, 0, 0, 15, 0, 0, 0, 3, 128, 0, 0, 0, 192, 0, 0, 0, 96, 0, 0, 0, 48, 0, 16, 0, 24, 0, 28, 0, 8, 0, 31, 0, 12, 0, 19, 192, 4, 0, 16, 224, 4, 0, 16, 56, 4, 0, 16, 30, 6, 0, 16, 7, 6, 0, 16, 7, 6, 0, 16, 14, 6, 0, 16, 56, 4, 0, 16, 224, 4, 0, 17, 128, 12, 0, 31, 0, 12, 0, 28, 0, 24, 0, 16, 0, 24, 0, 0, 0, 48, 0, 0, 0, 96, 0, 0, 0, 192, 0, 0, 1, 128, 0, 0, 15, 0, 0, 0, 28, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 224, 0, 0, 0, 252, 0, 0, 0, 15, 0, 0, 0, 3, 128, 0, 0, 0, 192, 0, 0, 0, 96, 0, 0, 0, 48, 0, 16, 0, 24, 0, 28, 0, 8, 0, 31, 0, 12, 0, 19, 192, 4, 0, 16, 224, 4, 0, 16, 56, 4, 0, 16, 30, 6, 0, 16, 7, 6, 0, 16, 7, 6, 0, 16, 14, 6, 0, 16, 56, 6, 0, 16, 224, 4, 0, 17, 128, 12, 0, 31, 0, 12, 0, 28, 0, 24, 0, 16, 0, 24, 0, 0, 0, 48, 0, 0, 0, 96, 0, 0, 0, 192, 1, 192, 1, 128, 0, 240, 7, 0, 0, 63, 252, 0, 0, 7, 192, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 224, 0, 0, 0, 252, 0, 0, 0, 15, 0, 0, 0, 3, 128, 0, 0, 0, 192, 0, 0, 0, 96, 0, 0, 0, 48, 0, 16, 0, 24, 0, 28, 0, 8, 0, 31, 0, 12, 0, 19, 192, 4, 0, 16, 224, 4, 0, 16, 56, 4, 0, 16, 30, 6, 0, 16, 7, 6, 0, 16, 7, 6, 0, 16, 14, 6, 96, 16, 56, 6, 32, 16, 224, 4, 48, 17, 128, 12, 16, 31, 0, 12, 16, 28, 0, 24, 24, 16, 0, 24, 12, 0, 0, 48, 6, 0, 0, 96, 3, 0, 0, 192, 1, 192, 1, 128, 0, 240, 7, 0, 0, 63, 252, 0, 0, 3, 192, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 224, 0, 0, 0, 252, 0, 0, 0, 15, 0, 0, 0, 3, 128, 0, 0, 0, 192, 0, 0, 0, 96, 0, 0, 0, 48, 0, 16, 0, 24, 16, 28, 0, 8, 48, 31, 0, 12, 48, 19, 192, 4, 32, 16, 224, 4, 96, 16, 56, 4, 96, 16, 30, 6, 96, 16, 7, 6, 96, 16, 7, 6, 96, 16, 14, 6, 32, 16, 56, 6, 32, 16, 224, 4, 32, 17, 128, 12, 48, 31, 0, 12, 16, 28, 0, 24, 24, 16, 0, 24, 12, 0, 0, 48, 6, 0, 0, 96, 3, 0, 0, 192, 1, 192, 1, 128, 0, 240, 7, 0, 0, 63, 252, 0, 0, 7, 192, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 224, 0, 0, 0, 252, 0, 0, 0, 15, 0, 1, 0, 3, 128, 3, 0, 0, 192, 6, 0, 0, 96, 12, 0, 0, 48, 24, 16, 0, 24, 24, 28, 0, 8, 48, 31, 0, 12, 48, 19, 192, 4, 32, 16, 224, 4, 32, 16, 56, 4, 96, 16, 30, 6, 96, 16, 7, 6, 96, 16, 7, 6, 96, 16, 14, 6, 32, 16, 56, 6, 32, 16, 224, 4, 32, 17, 128, 12, 48, 31, 0, 12, 16, 28, 0, 24, 24, 16, 0, 24, 12, 0, 0, 48, 6, 0, 0, 96, 3, 0, 0, 192, 1, 192, 1, 128, 0, 240, 7, 0, 0, 63, 252, 0, 0, 7, 192, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 224, 0, 0, 56, 252, 0, 0, 240, 15, 0, 1, 128, 3, 128, 3, 0, 0, 192, 6, 0, 0, 96, 12, 0, 0, 48, 24, 16, 0, 24, 24, 28, 0, 8, 48, 31, 0, 12, 48, 19, 192, 4, 32, 16, 224, 4, 32, 16, 56, 4, 96, 16, 30, 6, 96, 16, 7, 6, 96, 16, 7, 6, 96, 16, 14, 6, 32, 16, 56, 6, 32, 16, 224, 4, 32, 17, 128, 12, 48, 31, 0, 12, 16, 28, 0, 24, 24, 16, 0, 24, 12, 0, 0, 48, 6, 0, 0, 96, 3, 0, 0, 192, 1, 192, 1, 128, 0, 240, 7, 0, 0, 63, 252, 0, 0, 7, 192, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 3, 224, 0, 0, 63, 252, 0, 0, 224, 15, 0, 1, 128, 3, 128, 3, 0, 0, 192, 6, 0, 0, 96, 12, 0, 0, 48, 24, 16, 0, 24, 24, 28, 0, 8, 48, 31, 0, 12, 48, 19, 192, 4, 32, 16, 224, 4, 96, 16, 56, 4, 96, 16, 30, 6, 96, 16, 7, 6, 96, 16, 7, 6, 96, 16, 14, 6, 32, 16, 56, 6, 32, 16, 224, 4, 32, 17, 128, 12, 48, 31, 0, 12, 16, 28, 0, 24, 24, 16, 0, 24, 12, 0, 0, 48, 6, 0, 0, 96, 3, 0, 0, 192, 1, 192, 1, 128, 0, 240, 7, 0, 0, 63, 252, 0, 0, 7, 192, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 3, 224, 0, 0, 63, 252, 0, 0, 224, 15, 0, 1, 128, 3, 128, 3, 0, 0, 192, 6, 0, 0, 96, 12, 0, 0, 48, 24, 16, 0, 24, 24, 28, 0, 8, 48, 31, 0, 12, 48, 19, 192, 4, 32, 16, 224, 4, 96, 16, 56, 4, 96, 16, 30, 6, 96, 16, 7, 6, 96, 16, 7, 6, 96, 16, 14, 6, 32, 16, 56, 6, 32, 16, 224, 4, 32, 17, 128, 12, 48, 31, 0, 12, 16, 28, 0, 24, 24, 16, 0, 24, 12, 0, 0, 48, 6, 0, 0, 96, 3, 0, 0, 192, 1, 192, 1, 128, 0, 240, 7, 0, 0, 63, 252, 0, 0, 7, 192, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 3, 224, 0, 0, 63, 252, 0, 0, 224, 15, 0, 1, 128, 3, 128, 3, 0, 0, 192, 6, 0, 0, 96, 12, 0, 0, 48, 24, 16, 0, 24, 24, 28, 0, 8, 48, 31, 0, 12, 48, 19, 192, 4, 32, 16, 224, 4, 96, 16, 56, 4, 96, 16, 30, 6, 96, 16, 7, 6, 96, 16, 7, 6, 96, 16, 14, 6, 32, 16, 56, 6, 32, 16, 224, 4, 32, 17, 128, 12, 48, 31, 0, 12, 16, 28, 0, 24, 24, 16, 0, 24, 12, 0, 0, 48, 6, 0, 0, 96, 3, 0, 0, 192, 1, 192, 1, 128, 0, 240, 7, 0, 0, 63, 252, 0, 0, 7, 192, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 3, 224, 0, 0, 63, 252, 0, 0, 224, 15, 0, 1, 128, 3, 128, 3, 0, 0, 192, 6, 0, 0, 96, 12, 0, 0, 48, 24, 16, 0, 24, 24, 28, 0, 8, 48, 31, 0, 12, 48, 19, 192, 4, 32, 16, 224, 4, 96, 16, 56, 4, 96, 16, 30, 6, 96, 16, 7, 6, 96, 16, 7, 6, 96, 16, 14, 6, 32, 16, 56, 6, 32, 16, 224, 4, 32, 17, 128, 12, 48, 31, 0, 12, 16, 28, 0, 24, 24, 16, 0, 24, 12, 0, 0, 48, 6, 0, 0, 96, 3, 0, 0, 192, 1, 192, 1, 128, 0, 240, 7, 0, 0, 63, 252, 0, 0, 7, 192, 0, 0, 0, 0, 0 }
  };
  if (pos == 0) {
    display.fillRoundRect(UI_MIDDLE_X - 2 - 42, UI_MIDDLE_Y - 2, 36, 36, 10, 0);
    display.drawBitmap(UI_MIDDLE_X - 42, UI_MIDDLE_Y, frames[0], FRAME_WIDTH, FRAME_HEIGHT, 1);
  }
  if (pos == 1) {
    for (int frame = 0; frame < 28; frame++) {
      display.fillRoundRect(UI_MIDDLE_X - 2, UI_MIDDLE_Y - 2, 36, 36, 10, 0);
      display.drawBitmap(UI_MIDDLE_X, UI_MIDDLE_Y, frames[frame], FRAME_WIDTH, FRAME_HEIGHT, 1);
      display.display();
    }
  }
  if (pos == 2) {
    display.fillRoundRect(UI_MIDDLE_X - 2 + 42, UI_MIDDLE_Y - 2, 36, 36, 10, 0);
    display.drawBitmap(UI_MIDDLE_X + 42, UI_MIDDLE_Y, frames[0], FRAME_WIDTH, FRAME_HEIGHT, 1);
  }
}

void UI_container_left(bool isdisplay = true, int color = 1) {
  if (isdisplay) {
    if (color == 1) {
      display.fillRoundRect(UI_MIDDLE_X - 4 - 42, UI_MIDDLE_Y - 4, 40, 40, 10, 1);
      display.fillRoundRect(UI_MIDDLE_X - 2 - 42, UI_MIDDLE_Y - 2, 36, 36, 10, 0);
    } else if (color == 0) {
      display.fillRoundRect(UI_MIDDLE_X - 4 - 42, UI_MIDDLE_Y - 4, 40, 40, 10, 0);
    }
  } else {
    display.fillRoundRect(UI_MIDDLE_X - 4 - 42, UI_MIDDLE_Y - 4, 40, 40, 10, 0);
    display.display();
  }
}

void UI_container_right(bool isdisplay = true, int color = 1) {
  if (isdisplay) {
    if (color == 1) {
      display.fillRoundRect(UI_MIDDLE_X - 4 + 42, UI_MIDDLE_Y - 4, 40, 40, 10, 1);
      display.fillRoundRect(UI_MIDDLE_X - 2 + 42, UI_MIDDLE_Y - 2, 36, 36, 10, 0);
    } else if (color == 0) {
      display.fillRoundRect(UI_MIDDLE_X - 4 + 42, UI_MIDDLE_Y - 4, 40, 40, 10, 0);
    }
  } else {
    display.fillRoundRect(UI_MIDDLE_X - 4 + 42, UI_MIDDLE_Y - 4, 40, 40, 10, 0);
    display.display();
  }
}

void UI_container_center(bool isdisplay = true, int color = 1) {
  if (isdisplay) {
    if (color == 1) {
      display.fillRoundRect(UI_MIDDLE_X - 4, UI_MIDDLE_Y - 4, 40, 40, 10, 1);
      display.fillRoundRect(UI_MIDDLE_X - 2, UI_MIDDLE_Y - 2, 36, 36, 10, 0);
    } else if (color == 0) {
      display.fillRoundRect(UI_MIDDLE_X - 4, UI_MIDDLE_Y - 4, 40, 40, 10, 0);
    }
  } else {
    display.fillRoundRect(UI_MIDDLE_X - 4, UI_MIDDLE_Y - 4, 40, 40, 10, 0);
    display.display();
  }
}

void description(int index, bool selected = false) {
  display.fillRoundRect(4, 53, 120, 12, 10, 1);
  display.setTextColor(SSD1306_BLACK);
  display.setTextSize(1);
  if (index == 0) {
    if (!selected) {
      display.setCursor(22, 55);
      display.println(F("UNLOCK CABINET"));
    } else {
      for (int i = 0; i < 3; i++) {
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(22, 55);
        display.println(F("UNLOCK CABINET"));
        delay(100);
        display.setTextColor(SSD1306_BLACK);
        display.setCursor(22, 55);
        display.println(F("UNLOCK CABINET"));
        delay(100);
      }
    }
  }
  if (index == 1) {
    if (!selected) {
      display.setCursor(16, 55);
      display.println(F("TINKERING CORNER"));
    } else {
      for (int i = 0; i < 3; i++) {
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(16, 55);
        display.println(F("TINKERING CORNER"));
        delay(100);
        display.setTextColor(SSD1306_BLACK);
        display.setCursor(16, 55);
        display.println(F("TINKERING CORNER"));
        delay(100);
      }
    }
  }
  if (index == 2) {
    display.setCursor(25, 55);
    display.println(F("BATTERY LEVEL"));
  }
  if (index == 3) {
    display.setCursor(43, 55);
    display.println(F("SANDBOX"));
  }
}

// function to read buttons
int read_button_matrix() {
  // to read a button, the column must first be turned to ground.
  bool button_state = true;
  int button_pressed = 0;
  while (button_state) {
    for (int colIndex = 0; colIndex < colCount; colIndex++) {
      // Step 1: indicate which column it is currently at
      byte column_now = cols[colIndex];

      // Step 2: turn that column to ground
      pinMode(column_now, OUTPUT);
      digitalWrite(column_now, LOW);

      // Step 3: For that column, scan through every single row to see if a button is pressed.
      for (int rowIndex = 0; rowIndex < rowCount; rowIndex++) {
        byte row_now = rows[rowIndex];
        pinMode(row_now, INPUT_PULLUP);

        // Step 4: read if button is pressed or not
        if (digitalRead(row_now) == LOW) {
          button_pressed = handlebuttonpress(colIndex, rowIndex);
        }
      }
      // Step 5: Turn the column back to normal input pin. Repeat until all columns are scanned.
      pinMode(column_now, INPUT);
      button_state = false;
    }
  }
  return button_pressed;
}

int handlebuttonpress(int col, int row) {
  // define the map of the buttons
  const int button_map[3][3] = {
    { 1, 2, 3 },
    { 4, 5, 6 },
    { 7, 8, 9 }
  };

  // define what will happen if a button is pressed
  return button_map[col][row];
}

const int items_main[] = { 1, 2, 3, 4 };
const int items_top[] = { 1, 2, 3 };
int current_index_main = 0;
int current_index_top = 0;
const int numItems_main = sizeof(items_main) / sizeof(items_main[0]);
const int numItems_top = sizeof(items_top) / sizeof(items_top[0]);

void scroll_left() {
  // scenario 1: when it has not reach the far left end
  if (current_index_main > 0) {
    current_index_main--;
  }
  // scenario 2: when it reaches the far left end, wrap around
  else {
    current_index_main = numItems_main - 1;
  }
  pages_main(current_index_main);
}

void scroll_right() {
  // scenario 1: when it has not reach to the far right end
  if (current_index_main < numItems_main - 1) {
    current_index_main++;
  }
  // scenario 2: when it does reach the far right yet, wrap around
  else {
    current_index_main = 0;
  }
  pages_main(current_index_main);
}

void scroll_top_left() {
  // scenario 1: when it has not reach the far left end
  if (current_index_top > 0) {
    current_index_top--;
  } else {
    current_index_top = numItems_main - 1;
  }
  pages_top(current_index_top);
}

void scroll_top_right() {
  if (current_index_top < numItems_top - 1) {
    current_index_top++;
  } else {
    current_index_top = 0;
  }
  pages_top(current_index_top);
}

void pages_top(int index) {
  if (index == 0) {
    UI_container_left(true, 0);
    UI_container_right(true, 0);
    UI_container_center(true, 0);
    UI_cancel(0);
    UI_pause(2);
    UI_resume(1);
    display.display();
  } else if (index == 1) {
    UI_container_left(true, 0);
    UI_container_right(true, 0);
    UI_container_center(true, 0);
    UI_resume(0);
    UI_cancel(2);
    UI_pause(1);
    display.display();
  } else if (index == 2) {
    UI_container_left(true, 0);
    UI_container_right(true, 0);
    UI_container_center(true, 0);
    UI_resume(2);
    UI_pause(0);
    UI_cancel(1);
    display.display();
  }
}

void pages_main(int index) {
  description(index);
  // cabinet
  if (index == 0) {
    // first page  sandbox - cabinet - tinker
    UI_container_left();
    UI_sandbox(0);

    UI_container_right();
    UI_tinker_corner(2);

    UI_container_center();
    UI_cabinet(1);
    display.display();
  }
  // tinker
  else if (index == 1) {
    // second page  cabinet - tinker - battery
    UI_container_left();
    UI_cabinet(0);

    UI_container_right();
    UI_battery(2);

    UI_container_center();
    UI_tinker_corner(1);
    display.display();
  }
  // battery
  else if (index == 2) {
    // third page  tinker - battery - sandbox
    UI_container_left();
    UI_tinker_corner(0);

    UI_container_right();
    UI_sandbox(2);

    UI_container_center();
    UI_battery(1);
    display.display();
  }
  // sandbox
  else if (index == 3) {
    // forth page  battery - sandbox - cabinet
    UI_container_left();
    UI_battery(0);

    UI_container_right();
    UI_cabinet(2);

    UI_container_center();
    UI_sandbox(1);
    display.display();
  }
}

bool top_screen = false;

void setup() {
  // WIRE must be set up at all times when ESP32 is open
  Serial.begin(115200);
  Wire.setPins(I2C_SDA_PIN, I2C_SCL_PIN);
  Wire.begin();

  // Powering on the display
  while (!power_on) {
    if (read_button_matrix() == 9) {
      display.begin(SSD1306_SWITCHCAPVCC, SCREEN_I2C_ADDR);
      display.display();
      delay(500);
      display.clearDisplay();
      delay(500);
      power_on = true;
    }
  }

  // Connection to the WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    UI_wifi();
    delay(500);
  }
  display.clearDisplay();
  display.display();
  delay(500);
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);

  // Display Local IP Address
  display.setCursor(31, 45);
  display.println(F("IP Address:"));
  display.display();
  display.setCursor(30, 55);
  display.println(WiFi.localIP());
  display.display();

  // Indicator that WiFi is connected
  display.fillRoundRect(4, 18, 120, 12, 10, 1);
  display.setTextColor(SSD1306_BLACK);
  display.setTextSize(1);

  display.setCursor(16, 20);
  display.println(F("WiFi Connected!"));
  display.display();

  server.on("/post", HTTP_POST, handleRequest);
  server.begin();

  delay(3000);
  display.clearDisplay();
}

void loop() {
  int button_reading = read_button_matrix();
  if (button_reading == 0) {
    pages_main(current_index_main);
    button_reading = read_button_matrix();
  }

  // power off the OLED display
  else if (button_reading == 9) {
    display.clearDisplay();
    display.ssd1306_command(SSD1306_DISPLAYOFF);
    WiFi.disconnect();
    setup();
  }

  // user presses the select button
  if (button_reading == 5) {
    // scenario where tinker corner or sandbox is present
    if (current_index_main == 1 || current_index_main == 3) {
      for (int i = 0; i < 50; i = i + 2) {
        display.fillRoundRect(0, 0, 128, i, 10, 1);
        display.display();
      }
      top_screen = true;
      while (top_screen) {  // there will be a pull down screen when enabled
        pages_top(current_index_top);
        button_reading = read_button_matrix();

        // when user presses back button, breaks the while loop by defining top_screen to false
        if (button_reading == 7) {
          for (int j = 0; j <= 50; j = j + 2) {
            display.fillRect(0, 50 - j, 128, j, 0);
            display.display();
          }
          display.clearDisplay();
          pages_main(current_index_main);
          top_screen = false;
        }

        // scenario where user selects the option
        else if (button_reading == 5) {
          delay(500);
          display.clearDisplay();
          display.display();
          display.fillRoundRect(5, 5, 118, 54, 10, 1);
          display.fillRoundRect(7, 7, 114, 50, 10, 0);
          display.display();

          display.setTextColor(SSD1306_WHITE);
          display.setTextSize(1);
          display.setCursor(12, 30);
          delay(500);
          // if presses the play button, under tinker corner
          if (current_index_main == 1 && current_index_top == 0) {
            int response = request_tinkering_corner();
            if (response != 200) {
              display.println(F("ERROR..."));
              display.display();
            }
            else {
              display.println(F("SESTO GOING!"));
              display.display();
            }
            while (read_button_matrix() != 7) {
              delay(50);
            };
            display.clearDisplay();
            display.display();
            // Serial.println("should be going to tinker corner");
            top_screen = false;
          }

          // if presses the cancel button
          else if (current_index_top == 2) {
            int response = poster(retrieve_workid(), 0);
            if (response != 200) {
              display.println(F("Cancel failed"));
              display.display();
            }
            else {
              display.println(F("Task canceled"));
              display.display();
            }
            while (read_button_matrix() != 7) {
              delay(50);
            }
            display.clearDisplay();
            display.display();
            top_screen = false;
          }

          // if presses the resume/pause button
          else if (current_index_top == 1) {
            // retrieves the bot's status (pausing/moving)
            if (retrieve_botstatus() == "paused") {
              int response = poster(retrieve_workid(), 2);
              if (response != 200) {
                display.println(F("Resuming failed"));
                display.display();
              }
              else {
                display.println(F("Resuming"));
                display.display();
              }
              while (read_button_matrix() != 7) {
                delay(50);
              }
              display.clearDisplay();
              display.display();
              top_screen = false;
            }
            else {
              int response = poster(retrieve_workid(), 1);
              if (response != 200) {
                display.println(F("Pausing failed"));
                display.display();
              }
              else {
                display.println(F("Pausing"));
                display.display();
              }
              while (read_button_matrix() != 7) {
                delay(50);
              }
              display.clearDisplay();
              display.display();
              top_screen = false;
            }

            display.clearDisplay();
            top_screen = false;
          }

          // if presses the play button, under sandbox
          else if (current_index_top == 0 && current_index_main == 3) {
            int response = request_sandbox();
            if (response != 200) {
              display.println(F("ERROR..."));
              display.display();
            }
            else {
              display.println(F("SESTO GOING!"));
              display.display();
            }
            while (read_button_matrix() != 7) {
              delay(50);
            };
            display.clearDisplay();
            display.display();
            // show a window that says it is starting to move to sandbox
            top_screen = false;
          }
        }

        // when there is no input, idles
        else if (button_reading == 0) {
          pages_top(current_index_top);
          button_reading = read_button_matrix();
        }

        // when user presses the left button, scrolls left
        else if (button_reading == 4) {
          scroll_top_left();
        }

        // when user presses the right button, scrolls right
        else if (button_reading == 6) {
          scroll_top_right();
        }
      }
    }
    // scenario where cabinet is present
    if (current_index_main == 0) {
      int httpResponseCode = opencabinet();
      delay(500);
      display.clearDisplay();
      display.display();
      display.fillRoundRect(5, 5, 118, 54, 10, 1);
      display.fillRoundRect(7, 7, 114, 50, 10, 0);
      display.display();

      display.setTextColor(SSD1306_WHITE);
      display.setTextSize(1);
      display.setCursor(12, 30);
      if (httpResponseCode == 200) {
        display.println(F("Cabinet is Open!"));
        display.display();
        while (read_button_matrix() != 7) {
          delay(50);
        }
      }
      else {
        while (read_button_matrix() != 7) {
          display.println(F("Open Fail..."));
          display.display();
        }

      }
      display.clearDisplay();
      display.display();
    }
    // scenario where battery is present
    if (current_index_main == 5) {
      // retrieves battery level of sesto robot
      float battery_remote = battery_check_remote();

      while (read_button_matrix() != 7) {  // brings up a window that tells you the status of call
        display.fillRoundRect(5, 5, 123, 59, 10, 1);
        display.fillRoundRect(7, 7, 121, 27, 10, 0);
        display.display();

        display.setTextColor(SSD1306_WHITE);
        display.setTextSize(1);
        display.setCursor(12, 12);
        display.println(F("Remote: "));
        display.display();

        display.setCursor(20, 12);
        display.println((battery_remote / 3.7) * 100);
        display.display();

        // need to show battery level of sesto bot as well
      }

      display.clearDisplay();
      display.display();
      // retrieves battery level of remote control
    }
  }

  if (button_reading == 4) {
    scroll_left();
  }
  if (button_reading == 6) {
    scroll_right();
  }
}
