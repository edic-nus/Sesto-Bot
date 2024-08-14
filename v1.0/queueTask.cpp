#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "TP-Link_DE83";
const char* password = "46193346";

const char* serverName = "http://192.168.0.100/public/tasks";

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  WiFiClient client;
  HTTPClient http;
  http.begin(client, serverName);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "Bearer eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJpYXQiOjE3MTgxMjQ1MjcsIm5iZiI6MTcxODEyNDUyNywianRpIjoiNmVhYWY4MmItYzg1NC00MjNmLTk1ZDktOTMwNGYyZjVjOGRhIiwiZXhwIjoxNzIwNzE2NTI3LCJpZGVudGl0eSI6eyJncm91cF9uYW1lIjoicGFydG5lcl9hZG1pbiIsInVzZXJfbmFtZSI6InBhcnRuZXIiLCJmaXJzdF9uYW1lIjoiRElTVFJJQlVUT1IiLCJsYXN0X25hbWUiOiJBZG1pbiIsImVtYWlsIjoiIiwiY3JlYXRvciI6InNlc3RvIiwiaWQiOiJjZGFmYjc2Ni04OGMxLTQwYTktYjc2NC0xMWRjYTc5MGNkNDkiLCJub3RpZmljYXRpb25fc3Vic2NyaWJlcnMiOnsiYWxlcnQiOnsidWkiOnRydWUsImVtYWlsIjpmYWxzZX19LCJpc19lbWFpbF9vdHBfc2VudCI6ZmFsc2UsImlzX2VtYWlsX3ZlcmlmaWVkIjpmYWxzZX0sImZyZXNoIjpmYWxzZSwidHlwZSI6ImFjY2VzcyJ9.GSR2u5W-wVDyRdMmXZBwzd92hZnHS77_mwEwQV38Y9U");
  int httpResponseCode = http.POST("{\"amr_id\":15,\"workflow_id\":143}");
  http.end();
}

void loop() {
  
}