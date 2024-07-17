#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include "ArduinoJson.h"
#include <Arduino_JSON.h>
#include <Adafruit_NeoPixel.h>
#include <Wire.h>

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

void read_button_matrix() {
  // to read a button, the column must first be turned to ground.
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
        Serial.println(handlebuttonpress(colIndex, rowIndex));
      }
    }

    // Step 5: Turn the column back to normal input pin. Repeat until all columns are scanned.
    pinMode(column_now, INPUT);
  }
}

String handlebuttonpress(int col, int row) {
  // define the map of the buttons
  const char* button_map[3][3] = {
    { "B1-1", "B1-2", "B1-3" },
    { "B2-1", "B2-2", "B2-3" },
    { "B3-1", "B3-2", "B3-3" }
  };

  // define what will happen if a button is pressed
  Serial.print("column: ");
  Serial.println(col + 1);
  Serial.print("row: ");
  Serial.println(row + 1);
  return button_map[col][row];
}


void setup() {
  Serial.begin(115200);
  Serial.print("number of rows: ");
  Serial.println(rowCount);
  Serial.print("number of columns: ");
  Serial.println(colCount);
}



void loop() {
  read_button_matrix();
 
}
