#define LDR_PIN 0  
#define LED_PIN 13 

void setup() {
  Serial.begin(9600); 
  pinMode(LED_PIN, OUTPUT); 
}

void loop() {
  int ldrValue = analogRead(LDR_PIN); 
  Serial.print("LDR Value: "); 
  Serial.println(ldrValue);
  
  int ledBrightness = map(ldrValue, 0, 1023, 0, 255);
  
  analogWrite(LED_PIN, ledBrightness);
  Serial.println(ledBrightness);
  delay(100); 
}
