#define BLYNK_TEMPLATE_ID "TMPL6vl4GxGw7"
#define BLYNK_TEMPLATE_NAME "Trash"
#define BLYNK_TEMPLATE_AUTH "rd9tM8r-A9V8VysWrdQBlbBPnVUs_GnW"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <WebServer.h>

char ssid[] = "Legion";
char pass[] = "12345679";

char auth[] = BLYNK_TEMPLATE_AUTH;

#define POWER_PIN1 27
#define SIGNAL_PIN1 35
#define POWER_PIN2 14
#define SIGNAL_PIN2 33

#define RELAY_PIN 26

WebServer server(80);

void handleRoot() {
  String html = "<html><head><title>Jaring</title>";
  html += "<style>";
  html += "body { font-family: Arial, sans-serif; text-align: center; background-color: #f0f0f0; padding: 50px; }";
  html += "h1 { color: #333; }";
  html += "button { padding: 15px 25px; font-size: 24px; margin: 10px; cursor: pointer; }";
  html += ".on { background-color: #4CAF50; color: white; border: none; }";
  html += ".off { background-color: #f44336; color: white; border: none; }";
  html += "</style></head><body>";
  html += "<h1>Jaring</h1>";

  if (digitalRead(RELAY_PIN) == HIGH) {
    html += "<p>Jaring is <strong>ON</strong></p><p><a href=\"/off\"><button class=\"off\">Turn Off Jaring</button></a></p>";
  } else {
    html += "<p>Jaring is <strong>OFF</strong></p><p><a href=\"/on\"><button class=\"on\">Turn On Jaring</button></a></p>";
  }

  html += "</body></html>";

  server.send(200, "text/html", html);
}

void handleRelayOn() {
  digitalWrite(RELAY_PIN, HIGH);
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleRelayOff() {
  digitalWrite(RELAY_PIN, LOW);
  server.sendHeader("Location", "/");
  server.send(303);
}

void setup() {
  // Debug console
  Serial.begin(115200);

  // Begin Blynk
  Blynk.begin(auth, ssid, pass);

  // Print connection details
  Serial.println("Connecting to Blynk...");

  // For sensor
  pinMode(POWER_PIN1, OUTPUT);
  pinMode(POWER_PIN2, OUTPUT);
  digitalWrite(POWER_PIN1, LOW);
  digitalWrite(POWER_PIN2, LOW);

  // For relay
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  // Connect to Wi-Fi
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  
  // Print the IP address
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Start the server
  server.on("/", handleRoot);
  server.on("/on", handleRelayOn);
  server.on("/off", handleRelayOff);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  // Run Blynk
  Blynk.run();

  // Power the sensor and read the value
  digitalWrite(POWER_PIN1, HIGH);
  digitalWrite(POWER_PIN2, HIGH);
  delay(10);
  int value = analogRead(SIGNAL_PIN1);
  int value2 = analogRead(SIGNAL_PIN2);
  digitalWrite(POWER_PIN1, LOW);
  digitalWrite(POWER_PIN2, LOW);

  // Send the value to Blynk
  Blynk.virtualWrite(V0, value);
  Blynk.virtualWrite(V1, value2);
  if((value - value2) >= 250){
    digitalWrite(RELAY_PIN, HIGH);
  }else{
    digitalWrite(RELAY_PIN, LOW);
  }
  
  // Print the value to the serial monitor
  Serial.print("The water sensor value 1: ");
  Serial.println(value);
  Serial.print("The water sensor value 2: ");
  Serial.println(value2);

  // Handle client requests
  server.handleClient();

  // Add a delay to avoid spamming Blynk server
  delay(1000);
}
