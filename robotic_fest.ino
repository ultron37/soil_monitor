#include <WiFi.h>
#include <WebServer.h>

// ===== WIFI =====
const char* ssid = "KERALAVISION @vijaya krishnan";
const char* password = "12345678";

// ===== PINS =====
#define SOIL_PIN 32
#define RELAY_PIN 18

WebServer server(80);

int raw = 0;
bool pumpState = false;

void setup() {
  Serial.begin(115200);

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);  // OFF (important for your relay)

  // ===== WIFI =====
  WiFi.begin(ssid, password);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  // ===== WEB PAGE =====
  server.on("/", []() {
    String page = "<html><head><meta http-equiv='refresh' content='2'></head><body>";
    page += "<h1>Soil Raw: " + String(raw) + "</h1>";
    page += "<h2>Pump: " + String(pumpState ? "ON" : "OFF") + "</h2>";
    page += "</body></html>";
    server.send(200, "text/html", page);
  });

  server.begin();
}

void loop() {
  server.handleClient();

  // ===== READ SENSOR =====
  raw = analogRead(SOIL_PIN);

  // ===== SIMPLE RAW CONTROL =====
  if (raw > 2000) {
    pumpState = true;  // ON
  } else {
    pumpState = false;  // OFF
  }

  // ===== RELAY CONTROL (ACTIVE LOW) =====
  digitalWrite(RELAY_PIN, pumpState ? LOW : HIGH);

  // ===== SERIAL DEBUG =====
  Serial.print("Raw: ");
  Serial.print(raw);
  Serial.print("  Pump: ");
  Serial.println(pumpState ? "ON" : "OFF");

  delay(1000);
}