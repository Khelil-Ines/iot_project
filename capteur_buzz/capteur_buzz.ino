const int pinCapteur = 15;  // Broche connectée au capteur (choisis une broche valide sur l'ESP32)
const int pinBuzzer = 2;    // Broche connectée au buzzer
int etatCapteur = 0;

void setup() {
  Serial.begin(115200);   // Initialise la communication série
  pinMode(pinCapteur, INPUT); // Configure la broche du capteur comme entrée
  pinMode(pinBuzzer, OUTPUT); // Configure la broche du buzzer comme sortie
}

void loop() {
  etatCapteur = digitalRead(pinCapteur); // Lis l'état du capteur
  if (etatCapteur == HIGH) {
    Serial.println("Mouvement détecté !");
    digitalWrite(pinBuzzer, HIGH); // Active le buzzer
  } else {
    Serial.println("Pas de mouvement.");
    digitalWrite(pinBuzzer, LOW); // Désactive le buzzer
  }
  delay(1000); // Attends une seconde entre les lectures
}
