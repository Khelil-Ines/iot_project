
#include "DHT.h"


#define DHTPIN 2     // Pin numérique connectée au capteur DHT

// Utiliser le DHT11
#define DHTTYPE DHT11   // DHT 11

// Initialiser le capteur DHT
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  // Initialiser le moniteur série
  Serial.begin(9600);
  Serial.println(F("DHT11 test!"));

  // Commencer la communication avec le capteur DHT
  dht.begin();
}

void loop() {
  // Attendre quelques secondes entre les mesures
  delay(2000);

  // Lire l'humidité
  float h = dht.readHumidity();
  // Lire la température en Celsius
  float t = dht.readTemperature();
  // Lire la température en Fahrenheit
  float f = dht.readTemperature(true);

  // Vérifier les erreurs de lecture
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Calculer l'indice de chaleur en Fahrenheit
  float hif = dht.computeHeatIndex(f, h);
  // Calculer l'indice de chaleur en Celsius
  float hic = dht.computeHeatIndex(t, h, false);

  // Afficher les valeurs mesurées dans le moniteur série
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(f);
  Serial.print(F("°F  Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C "));
  Serial.print(hif);
  Serial.println(F("°F"));
}
