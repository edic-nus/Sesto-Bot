//#include "WiFi101.h"
#include <WiFi.h>
#include <WebServer.h>
#include "ArduinoJson.h"
#include <HTTPClient.h>
#include <ESP32Ping.h>

// Replace with your network credentials
const char *ssid = "TP-Link_DE83";
const char *password = "46193346";

bool isSleep = true;
const char* sestoIP = "192.168.0.100";
const char* sestoServer = "http://192.168.0.100/public/tasks";
// "0" -> home location (w recovery), "1" -> other side of HUB
String currLocation = "0";
int count = 0;

//intialising timer
hw_timer_t *timerCheck = NULL;
void IRAM_ATTR Timer0_ISR() {
  //if (!isSleep) {
    //check if SESTO is online
    if (!Ping.ping(sestoIP)) {
      isSleep = true;
      //sesto has turned offline must reset to home location, change currloc to "0"
      currLocation = "0";
      //stop timer
      //timerStop(timerCheck);
    } else {
      isSleep = false;
    }
    count++;
}
// Create a web server on port 80
WebServer server(80);

WiFiClient client;
HTTPClient http;

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
  String location = doc["doorTo"];

  if (action == "openDoor") //sesto calling server to open the door
  {
    handleOpenDoor();
    server.send(200, "application/json");
    delay(1000);
  } else if (action == "checkOnline") { //for us to check state for debugging
    String sleep;
    if (isSleep) {
      sleep = "1";
    } else {
      sleep = "0";
    }
    String response = sleep + " " + currLocation + " " + count;
    server.send(200, "application/json", response);
  } else if (location != currLocation) { //meaning sesto diff loc than loc its called to
    http.begin(client, sestoServer);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", "Bearer eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJpYXQiOjE3MTgxMjQ1MjcsIm5iZiI6MTcxODEyNDUyNywianRpIjoiNmVhYWY4MmItYzg1NC00MjNmLTk1ZDktOTMwNGYyZjVjOGRhIiwiZXhwIjoxNzIwNzE2NTI3LCJpZGVudGl0eSI6eyJncm91cF9uYW1lIjoicGFydG5lcl9hZG1pbiIsInVzZXJfbmFtZSI6InBhcnRuZXIiLCJmaXJzdF9uYW1lIjoiRElTVFJJQlVUT1IiLCJsYXN0X25hbWUiOiJBZG1pbiIsImVtYWlsIjoiIiwiY3JlYXRvciI6InNlc3RvIiwiaWQiOiJjZGFmYjc2Ni04OGMxLTQwYTktYjc2NC0xMWRjYTc5MGNkNDkiLCJub3RpZmljYXRpb25fc3Vic2NyaWJlcnMiOnsiYWxlcnQiOnsidWkiOnRydWUsImVtYWlsIjpmYWxzZX19LCJpc19lbWFpbF9vdHBfc2VudCI6ZmFsc2UsImlzX2VtYWlsX3ZlcmlmaWVkIjpmYWxzZX0sImZyZXNoIjpmYWxzZSwidHlwZSI6ImFjY2VzcyJ9.GSR2u5W-wVDyRdMmXZBwzd92hZnHS77_mwEwQV38Y9U");
    //queue task according to location that it is called to
    if (location == "0") {
      int httpResponseCode = http.POST("{\"amr_id\":15,\"workflow_id\":143}");
    } else {
      int httpResponseCode = http.POST("{\"amr_id\":15,\"workflow_id\":143}");

    }
    http.end();
    server.send(200, "application/json");
    delay(1000);
  } else {
    server.send(400, "application/json", "{\n\t\"status\":\"error\",\n\t\"data\":{\n\t\t\"message\":\"Invalid Action\"\n\t}\n}\n");
  }
  if (isSleep) { //meaning first call since sesto is turned on
    isSleep = false; //reset prog to let it know sesto is online now
    //timerStart(timerCheck); //resume timer to check until sesto goes back to sleep
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
  //timer config
  timerCheck = timerBegin(0, 80, true);
  timerAttachInterrupt(timerCheck, &Timer0_ISR, true);
  timerAlarmWrite(timerCheck, 1000000, true);
  timerAlarmEnable(timerCheck);
}

void loop()
{
  // Handle client requests
  server.handleClient();
}