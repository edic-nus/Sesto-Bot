const int batteryPin = 6; // ADC pin connected to the voltage divider
float R1 = 10000.0; // Resistance of R1 (in ohms)
float R2 = 10000.0;  // Resistance of R2 (in ohms)
float maxAdcValue = 4095.0; // Maximum value for the 12-bit ADC
float referenceVoltage = 3.3; // Reference voltage for the ESP32 ADC

void setup() {
  Serial.begin(115200);
  analogReadResolution(12); // Set ADC resolution to 12 bits
}

void loop() {
  int adcValue = analogRead(batteryPin);
  float voltage = (adcValue / maxAdcValue) * referenceVoltage;
  float batteryVoltage = voltage * (R1 + R2) / (R2); // Calculate actual battery voltage

  Serial.print("ADC Value: ");
  Serial.print(adcValue);
  Serial.print(" - Battery Voltage: ");
  Serial.println(batteryVoltage);
  delay(500);
}
