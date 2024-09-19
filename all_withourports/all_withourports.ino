#include <WiFi.h>
#include <WebServer.h>
#include "ThingSpeak.h"
#include "ESP_Mail_Client.h"
#include "DHT.h"
#include "HTTPClient.h"
#include "time.h"
#include <ArduinoJson.h>

#define GMAIL_SMTP_SERVER "smtp.gmail.com"
#define GMAIL_SMTP_USERNAME "******"
#define GMAIL_SMTP_PASSWORD "******"
#define GMAIL_SMTP_PORT 465

#define PIR 15
#define LED 2
#define BUZZER 16
#define LM35 13
#define DHTTYPE DHT11
#define dht_dpin 14

float threshold = 25.00;
String Fire_alarm_status = "";
String Motion_sensor_status = "";

DHT dht(dht_dpin, DHTTYPE);
const char *ssid = "******";
const char *password = ""******"";

// ThingSpeak
unsigned long myChannelNumber = ******;
const char * myWriteAPIKey = "******";
unsigned long lastTime = 0;
unsigned long timerDelay = 30000;
WiFiClient client;

WebServer server(80);

// Email Data
SMTPSession smtp;
ESP_Mail_Session session;
SMTP_Message message;
char* fire_mail = "<!DOCTYPE html><html>Fire detected in your house!</html>";
char* motion_mail = "<!DOCTYPE html><html>Motion detected in your house!</html>";

bool fire_detected = 0;
String page = "";
int HOME_LED = 0;
int motion_detected = 0;
int mvt = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("Code started"); 

  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  pinMode(PIR, INPUT);
  pinMode(dht_dpin, INPUT);
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);

  dht.begin();
  ThingSpeak.begin(client);
  delay(50);
  WiFi.mode(WIFI_STA);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  page = "<head><style>body {font-family: sans-serif;background-color: #98ddca;}h1 {text-align: center;color: white;font-size: 3.5em;}h2 {color: white;font-size: 2.5em;margin-bottom: 7%;}.card {background-color: #ffaaa7;border-radius: 10px;padding: 5%;margin: 20px 10%;}.button {background-color: #ffd3b4;color: black;border-radius: 10px;padding: 10px 12%;margin: 0 2% 0 0;text-decoration: none;}p {color: black;font-size: 1.3rem;}</style></head><h1>IoT Project</h1><div class=\"card\"><h2>Home Automation</h2><p><a class=\"button\" href=\"LEDOn\">ON</a><a class=\"button\" href=\"LEDOff\">OFF</a></p></div><br /><div class=\"card\"><h2>Home Security</h2><p id=\"mdata\"></p></div><br /><div class=\"card\"><h2>Fire Security</h2><p id=\"fdata\"></p></div><script>var mdat = setInterval(function () {loadValues_m(\"datam.txt\", mupdateValues);}, 1000);function loadValues_m(url, callback) {var xhttp = new XMLHttpRequest();xhttp.onreadystatechange = function () {if(this.readyState == 4 && this.status == 200){callback.apply(xhttp);}};xhttp.open(\"GET\", url, true);xhttp.send();}function mupdateValues() {document.getElementById(\"mdata\").innerHTML = this.responseText; setcolor();}var fdat = setInterval(function () {loadValues_f(\"dataf.txt\", fupdateValues);}, 1000);function loadValues_f(url, callback) {var xhttp = new XMLHttpRequest();xhttp.onreadystatechange = function () {if (this.readyState == 4 && this.status == 200) {callback.apply(xhttp);}};xhttp.open(\"GET\", url, true);xhttp.send();}function fupdateValues() {document.getElementById(\"fdata\").innerHTML = this.responseText; setcolor();}function setcolor() {var md = document.getElementById(\"mdata\");if (md.innerHTML == \"Everything is safe!\") {md.style.color = \"green\";} else {md.style.color = \"red\";}var fd = document.getElementById(\"fdata\");if (fd.innerHTML == \"Everything is safe!\") {fd.style.color = \"green\";}else{fd.style.color = \"red\";}}</script>";

  server.on("/", []() {
    server.send(200, "text/html", page);
  });
  server.on("/LEDOn", []() {
    server.send(200, "text/html", page);
    digitalWrite(LED, HIGH);
    HOME_LED = 1;
    delay(1000);
  });
  server.on("/LEDOff", []() {
    server.send(200, "text/html", page);
    digitalWrite(LED, LOW);
    HOME_LED = 0;
    delay(1000);
  });
  server.on("/dataf.txt", []() {
    if (fire_detected) Fire_alarm_status = "Come fast! Fire in your house";
    else Fire_alarm_status = "Everything is safe!";
    server.send(200, "text/html", Fire_alarm_status);
    Fire_alarm_status = "";
  });
  server.on("/datam.txt", []() {
    if (motion_detected == 1) Motion_sensor_status = "Come fast! Someone is in your house";
    else Motion_sensor_status = "Everything is safe!";
    server.send(200, "text/html", Motion_sensor_status);
    Motion_sensor_status = "";
  });

  server.begin();
  Serial.println("Web server started!");

  // Configure email session
  session.server.host_name = GMAIL_SMTP_SERVER;
  session.server.port = GMAIL_SMTP_PORT;
  session.login.email = GMAIL_SMTP_USERNAME;
  session.login.password = GMAIL_SMTP_PASSWORD;
 session.login.user_domain = "";
  Serial.println("pb1!");

  // Configure email message
  message.sender.name = "ESP32";
  message.sender.email = GMAIL_SMTP_USERNAME;
  message.subject = "[IMPORTANT] Alert";
  message.addRecipient("Recipient", "testaouitest@gmail.com");
  message.html.charSet = "utf-8";
  message.priority = esp_mail_smtp_priority::esp_mail_smtp_priority_high;
  message.response.notify = esp_mail_smtp_notify_success | esp_mail_smtp_notify_failure | esp_mail_smtp_notify_delay;
  Serial.println("pb2!");

}

void loop() {
  if ((millis() - lastTime) > timerDelay) {
    server.handleClient();
    int motion = digitalRead(PIR);  // Taking readings from PIR sensor

    if (motion == HIGH) {
      Serial.println("Mouvement détecté !");
      digitalWrite(BUZZER, HIGH); // Turn on the buzzer
      digitalWrite(LED, LOW);
      Serial.println("Buzzer");
      Serial.println("Buzzer");
      motion_detected = 1; // Set motion_detected to 1
      mvt = 1;

      message.html.content = motion_mail;  // Update email content

      if (!smtp.connect(&session)) {
        Serial.println("Failed to connect to the mail server");
        return;
      }

      if (!MailClient.sendMail(&smtp, &message)) {
        Serial.println("Failed to send Email");
        Serial.println(smtp.errorReason().c_str());
      }

      smtp.closeSession();
      Serial.println("Mail sent!");
      delay(5000);
    } else {
      Serial.println("Pas de mouvement.");
      digitalWrite(BUZZER, LOW); // Turn off the buzzer
      motion_detected = 0; // Set motion_detected to 0
    }

    Serial.print("Motion: ");  // Printing Values
    Serial.println(motion_detected);
    Serial.print("Mvt: ");  // Printing Values
    Serial.println(mvt);

    lastTime = millis();
  }
}
