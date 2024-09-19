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

// #define LDR_PIN 0  
// #define LED_PIN 16
#define LED_temp 4 
#define PIR 12
#define LED 2
#define BUZZER 14
#define LM35 13
#define DHTTYPE DHT11
#define dht_dpin 15

float threshold = 1.5;
String Fire_alarm_status = "";
String Motion_sensor_status = "";

DHT dht(dht_dpin, DHTTYPE);

const char *ssid = "****";
const char *password = "******";

// ThingSpeak
unsigned long myChannelNumber = ******;
const char * myWriteAPIKey = "******";
unsigned long lastTime = 0;
unsigned long timerDelay = 10000;
WiFiClient client;

WebServer server(80);

// Email Data
SMTPSession smtp;  
ESP_Mail_Session session;
SMTP_Message message;

char* fire_mail = "<!DOCTYPE html><html lang=\"en\" xmlns=\"http://www.w3.org/1999/xhtml\" xmlns:o=\"urn:schemas-microsoft-com:office:office\"><head> <meta charset=\"UTF-8\"> <meta name=\"viewport\" content=\"width=device-width,initial-scale=1\"> <meta name=\"x-apple-disable-message-reformatting\"> <title></title> <!--[if mso]><noscript><xml><o:OfficeDocumentSettings><o:PixelsPerInch>96</o:PixelsPerInch></o:OfficeDocumentSettings></xml></noscript><![endif]--> <style> table, td, div, h1, p { font-family: Arial, sans-serif; } </style></head><body style=\"margin:0;padding:0;\"> <table role=\"presentation\" style=\"width:100%;border-collapse:collapse;border:0;border-spacing:0;background:#ffffff;\"> <tr> <td align=\"center\" style=\"padding:0;\"> <table role=\"presentation\" style=\"width:602px;border-collapse:collapse;border:1px solid #cccccc;border-spacing:0;text-align:left;\"> <tr> <td align=\"center\" style=\"padding:20px 0 10px 0;background:#70bbd9;\"> <img src=\"https://i.ibb.co/x8X4KFf/Pin-Clipart-com-jail-clipart-12844.png\" width=\"150\" style=\"height:auto;display:block;\" /> </td> </tr> <tr> <td align=\"center\" style=\"background:#70bbd9;\"> <p style=\"font-size: 3rem;\"> Security Alert!</p> </td> </tr> <tr> <td style=\"padding:36px 30px 42px 30px;\"> <table role=\"presentation\" style=\"width:100%;border-collapse:collapse;border:0;border-spacing:0;\"> <tr> <td style=\"padding:0 0 0 0;color:#153643;\"> <h1 style=\"font-size:24px;margin:0 0 20px 0;font-family:Arial,sans-serif;\"> Emergency!!</h1> <p style=\"margin:0 0 12px 0;font-size:1.75rem;line-height:24px;font-family:Arial,sans-serif;color:red;\"> Fire detected at your house. Hurry!</p> </td> </tr> <tr> <td align=\"center\"> <img src=\" https://i.ibb.co/GWsHYP4/Pngtree-burning-fire-vector-element-5816475.png\" width=\"200\" style=\"height:auto;display:block;\" /> </td> </tr> </table> </td> </tr> <tr> <td style=\"padding:30px;background:#ee4c50;\"> <table role=\"presentation\" style=\"width:100%;border-collapse:collapse;border:0;border-spacing:0;font-size:9px;font-family:Arial,sans-serif;\"> <tr> <td style=\"padding:0;width:50%;\" align=\"left\"> <p style=\"margin:0;font-size:14px;line-height:16px;font-family:Arial,sans-serif;color:#ffffff;\"> Team Electricians<br />IoT Project </p> </td> </tr> </table> </td> </tr> </table> </td> </tr> </table></body></html>";
char* motion_mail = "<!DOCTYPE html><html lang=\"en\" xmlns=\"http://www.w3.org/1999/xhtml\" xmlns:o=\"urn:schemas-microsoft-com:office:office\"><head> <meta charset=\"UTF-8\"> <meta name=\"viewport\" content=\"width=device-width,initial-scale=1\"> <meta name=\"x-apple-disable-message-reformatting\"> <title></title> <!--[if mso]><noscript><xml><o:OfficeDocumentSettings><o:PixelsPerInch>96</o:PixelsPerInch></o:OfficeDocumentSettings></xml></noscript><![endif]--> <style> table, td, div, h1, p { font-family: Arial, sans-serif; } </style></head><body style=\"margin:0;padding:0;\"> <table role=\"presentation\" style=\"width:100%;border-collapse:collapse;border:0;border-spacing:0;background:#ffffff;\"> <tr> <td align=\"center\" style=\"padding:0;\"> <table role=\"presentation\" style=\"width:602px;border-collapse:collapse;border:1px solid #cccccc;border-spacing:0;text-align:left;\"> <tr> <td align=\"center\" style=\"padding:20px 0 10px 0;background:#70bbd9;\"> <img src=\"https://i.ibb.co/x8X4KFf/Pin-Clipart-com-jail-clipart-12844.png\" width=\"150\" style=\"height:auto;display:block;\" /> </td> </tr> <tr> <td align=\"center\" style=\"background:#70bbd9;\"> <p style=\"font-size: 3rem;\"> Security Alert!</p> </td> </tr> <tr> <td style=\"padding:36px 30px 42px 30px;\"> <table role=\"presentation\" style=\"width:100%;border-collapse:collapse;border:0;border-spacing:0;\"> <tr> <td style=\"padding:0 0 0 0;color:#153643;\"> <h1 style=\"font-size:24px;margin:0 0 20px 0;font-family:Arial,sans-serif;\"> Emergency!!</h1> <p style=\"margin:0 0 12px 0;font-size:1.75rem;line-height:24px;font-family:Arial,sans-serif;color:red;\"> Someone is inside your house. Hurry!</p> </td> </tr> <tr> <td align=\"center\" style=\"padding:20px 0 0\"> <img src=\" https://i.ibb.co/f1W92Z9/pngegg.png\" width=\"200\" style=\"height:auto;display:block;\" /> </td> </tr> </table> </td> </tr> <tr> <td style=\"padding:30px;background:#ee4c50;\"> <table role=\"presentation\" style=\"width:100%;border-collapse:collapse;border:0;border-spacing:0;font-size:9px;font-family:Arial,sans-serif;\"> <tr> <td style=\"padding:0;width:50%;\" align=\"left\"> <p style=\"margin:0;font-size:14px;line-height:16px;font-family:Arial,sans-serif;color:#ffffff;\"> Team Electricians<br />IoT Project </p> </td> </tr> </table> </td> </tr> </table> </td> </tr> </table></body></html>";



bool fire_detected = 0;
String page = "";
int motion_detected = 0; 

void setup() {
  Serial.begin(9600);
  Serial.println("Code started");  

  pinMode(LED, OUTPUT);
//  pinMode(LED_PIN, OUTPUT); 
  pinMode(LED_temp, OUTPUT);
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
    delay(1000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

page = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><title>Smarty</title><link href=\"https://fonts.googleapis.com/css2?family=Roboto:wght@400;700&display=swap\" rel=\"stylesheet\"><link rel=\"stylesheet\" href=\"https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.0.0-beta3/css/all.min.css\"><style>body {font-family: 'Roboto', sans-serif;background-color: #f8f9fa;margin: 0;padding: 0;display: flex;flex-direction: column;align-items: center;}h1 {color: #343a40;font-size: 3em;margin-top: 20px;margin-bottom: 40px;}.container {display: flex;flex-wrap: wrap;justify-content: center;padding: 20px;}.card {background-color: #ffffff;border-radius: 15px;padding: 30px;margin: 15px;width: 320px;box-shadow: 0 6px 15px rgba(0, 0, 0, 0.1);transition: transform 0.3s, box-shadow 0.3s;}.card:hover {transform: translateY(-10px);box-shadow: 0 10px 25px rgba(0, 0, 0, 0.2);}.card h2 {font-size: 1.8em;margin-bottom: 20px;}.home-automation {background-color: #e3f2fd;}.home-security {background-color: #fff3e0;}.fire-security {background-color: #fce4ec;}.temperature-control {background-color: #e8f5e9;}.energy-monitoring {background-color: #f9fbe7;}.button {color: white;border: none;border-radius: 5px;padding: 10px 20px;text-align: center;text-decoration: none;display: inline-block;font-size: 1rem;margin: 5px 5px 15px 0;cursor: pointer;transition: background-color 0.3s;}.button.on {background-color: #4caf50;}.button.off {background-color: #f44336;}.button.on:hover {background-color: #388e3c;}.button.off:hover {background-color: #d32f2f;}p {color: #555;font-size: 1.2rem;}.status {font-weight: bold;display: flex;align-items: center;}.status-icon {margin-right: 10px;font-size: 1.5em;}.status.safe .status-icon {color: green;}.status.warning .status-icon {color: red;}.footer {margin-top: 40px;padding: 20px;background-color: #343a40;color: #ffffff;text-align: center;width: 100%;}.footer a {color: #ffffff;text-decoration: none;margin: 0 10px;}.footer a:hover {text-decoration: underline;}</style></head><body><h1>Smarty Control</h1><div class=\"container\"><div class=\"card home-automation\"><h2>Home Automation</h2><p><a class=\"button on\" href=\"LEDOn\">ON</a><a class=\"button off\" href=\"LEDOff\">OFF</a></p></div><div class=\"card home-security\"><h2>Home Security</h2><p id=\"mdata\" class=\"status safe\"><span class=\"status-icon\"><i class=\"fas fa-check-circle\"></i></span> Loading...</p></div><div class=\"card fire-security\"><h2>Fire Security</h2><p id=\"fdata\" class=\"status safe\"><span class=\"status-icon\"><i class=\"fas fa-check-circle\"></i></span> Loading...</p></div></div><div class=\"footer\"><p><a href=\"#\">Asma Bouziri</a> | <a href=\"#\">Aya Ghattas</a> | <a href=\"#\">Faten Borchani</a> | <a href=\"#\">Ghofrane Mannai</a> | <a href=\"#\">Ines Khalil</a></p></div><script>function loadValues(url, callback) {var xhttp = new XMLHttpRequest();xhttp.onreadystatechange = function () {if (this.readyState == 4 && this.status == 200) {callback.apply(xhttp);}};xhttp.open(\"GET\", url, true);xhttp.send();}function updateValues_m() {document.getElementById(\"mdata\").innerHTML = this.responseText;setcolor(\"mdata\");}function updateValues_f() {document.getElementById(\"fdata\").innerHTML = this.responseText;setcolor(\"fdata\");}function updateValues_t() {document.getElementById(\"tdata\").innerHTML = this.responseText;setcolor(\"tdata\");}function updateValues_e() {document.getElementById(\"edata\").innerHTML = this.responseText;setcolor(\"edata\");}function setcolor(elementId) {var el = document.getElementById(elementId);var statusIcon = el.querySelector('.status-icon');if (el.innerHTML.includes(\"Everything is safe!\") || el.innerHTML.includes(\"Normal\")) {el.classList.add('safe');el.classList.remove('warning');statusIcon.innerHTML = '<i class=\"fas fa-check-circle\"></i>';} else {el.classList.add('warning');el.classList.remove('safe');statusIcon.innerHTML = '<i class=\"fas fa-exclamation-circle\"></i>';}}setInterval(function () { loadValues(\"datam.txt\", updateValues_m); }, 1000);setInterval(function () { loadValues(\"dataf.txt\", updateValues_f); }, 1000);setInterval(function () { loadValues(\"datat.txt\", updateValues_t); }, 1000);setInterval(function () { loadValues(\"datae.txt\", updateValues_e); }, 1000);</script></body></html>";


  server.on("/", []() {
    server.send(200, "text/html", page);
  });
  server.on("/LEDOn", []() {
    server.send(200, "text/html", page);
    digitalWrite(LED, HIGH);
    Serial.println("led 1 ON");
    delay(1000);
  });
  server.on("/LEDOff", []() {
    server.send(200, "text/html", page);
    digitalWrite(LED, LOW);
    Serial.println("led 0 OFF");
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

  // Configure email message
  message.sender.name = "ESP32";
  message.sender.email = GMAIL_SMTP_USERNAME;
  message.subject = "[IMPORTANT] Alert";
  message.addRecipient("Recipient", "testaouitest@gmail.com");
  message.html.charSet = "utf-8";
  message.priority = esp_mail_smtp_priority::esp_mail_smtp_priority_high;
  message.response.notify = esp_mail_smtp_notify_success | esp_mail_smtp_notify_failure | esp_mail_smtp_notify_delay;
}

void loop() {
  //   int ldrValue = analogRead(LDR_PIN); // Read the value from the LDR
  // Serial.print("LDR Value: "); // Print the LDR value to the Serial Monitor
  // Serial.println(ldrValue);
  // // Map the LDR value (0-1023) to LED brightness (0-255)
  // int ledBrightness = map(ldrValue, 0, 1023, 0, 255);
  
  // // Set the LED brightness
  // analogWrite(LED_PIN, ledBrightness);
  // Serial.println(ledBrightness);
  
  if ((millis() - lastTime) > timerDelay) {
    server.handleClient();
    digitalWrite(LED_temp, LOW);
    motion_detected = digitalRead(PIR); 
    //int motion = digitalRead(PIR);  // Taking readings from PIR sensor

  
   

    float Temperature = dht.readTemperature();  
    float Humidity = dht.readHumidity();
    if (isnan(Temperature)) {
      Temperature = 0;
    }
    if (isnan(Humidity)) {
      Humidity = 0;
    }

    fire_detected = Temperature > threshold;

    if (motion_detected  && fire_detected) {  
      digitalWrite(LED_temp, HIGH);
      Serial.println("Light for temperature !");
      message.html.content = fire_mail; 
      message.html.content = motion_mail;  

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

    } else if (motion_detected ) {  
      message.html.content = motion_mail;  
      if (!smtp.connect(&session)) {
        Serial.println("Failed to connect to the mail server");
        return;
      }

      if (!MailClient.sendMail(&smtp, &message)) {
        Serial.println("Failed to send Email");
        Serial.println(smtp.errorReason().c_str());
      }

      smtp.closeSession();
      delay(5000);
      Serial.println("Mail sent!");
    } else if (fire_detected) {  
      digitalWrite(LED_temp, HIGH);
      Serial.println("Light for temperature !");
      message.html.content = fire_mail;  

      if (!smtp.connect(&session)) {
        Serial.println("Failed to connect to the mail server");
        return;
      }

      if (!MailClient.sendMail(&smtp, &message)) {
        Serial.println("Failed to send Email");
        Serial.println(smtp.errorReason().c_str());
      }

      smtp.closeSession();
      delay(5000);
    } else {
      digitalWrite(LED_temp, LOW);
 
    }

    if (motion_detected) {
      Serial.println("Mouvement détecté !");
      digitalWrite(BUZZER, HIGH); 
      Serial.println("Buzzer");
      delay(1000);
      motion_detected = 1; 
      digitalWrite(BUZZER, LOW); 
    } else {
      Serial.println("Pas de mouvement.");
      digitalWrite(BUZZER, LOW); 
      motion_detected = 0; 
    }

    Serial.print("Motion: "); 
    Serial.println(motion_detected);
    Serial.print("Temperature: ");
    Serial.println(Temperature);
    Serial.print("Humidity: ");
    Serial.println(Humidity);


    // Adding DHT11 readings to ThingSpeak
    ThingSpeak.setField(1, Temperature); 
    ThingSpeak.setField(2, motion_detected); 
    ThingSpeak.setField(3, Humidity); 

    int result = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

    if (result == 200) {
      Serial.println("Channel update successful.");
      delay(500);
    } else {
      Serial.println("Problem updating channel. HTTP error code " + String(result));
    }
    lastTime = millis();
  }
}
