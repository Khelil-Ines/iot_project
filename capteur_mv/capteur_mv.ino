// Remplace le numéro de la broche par celle connectée au capteur
const int pinCapteur = 15;
int etatCapteur = 0;

void setup() {
  Serial.begin(115200); // Initialise la communication série
  pinMode(pinCapteur, INPUT); // Configure la broche comme entrée
}

void loop() {
  etatCapteur = digitalRead(pinCapteur); // Lis l'état du capteur
  if (etatCapteur == HIGH) {
    Serial.println("Mouvement détecté !");
  } else {
    Serial.println("Pas de mouvement.");
  }
  delay(1000); // Attends une seconde entre les messages
}
