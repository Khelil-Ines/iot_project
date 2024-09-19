const int pinBuzzer = 2; // Broche connectée au buzzer

void setup() {
  pinMode(pinBuzzer, OUTPUT); // Configure la broche du buzzer comme sortie
}

void loop() {
  digitalWrite(pinBuzzer, HIGH); // Active le buzzer
  delay(1000); // Le buzzer reste activé pendant 1 seconde
  digitalWrite(pinBuzzer, LOW); // Désactive le buzzer
  delay(1000); // Pause de 1 seconde avant de réactiver
}
