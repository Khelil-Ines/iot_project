#define LM35_PIN 2  // LM35 connected to analog pin A0

void setup() {
  Serial.begin(9600); // Start the serial communication
}

void loop() {
  int lm35Value = analogRead(LM35_PIN); // Read the value from the LM35
  float voltage = lm35Value * (5.0 / 1023.0); // Convert the analog value to voltage
  float temperatureC = voltage * 100.0; // Convert the voltage to temperature in Celsius

  // Print the temperature to the Serial Monitor
  Serial.print("Temperature: ");
  Serial.print(temperatureC);
  Serial.println(" °C");
  
  delay(1000); // Wait for 1 second before reading again
}
