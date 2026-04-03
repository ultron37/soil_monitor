#include <WiFi.h>
#include <WebServer.h>

#define CONTROL_PIN 5
#define SENSOR_PIN  32

const char* ssid     = "KERALAVISION @vijaya krishnan";
const char* password = "12345678";

WebServer server(80);
bool relayState = false;

// ── /data  — JSON endpoint ────────────────────────────────────────────────
void handleData() {
  int raw = analogRead(SENSOR_PIN);
  int pct = map(raw, 4095, 0, 0, 100);

  String json = "{";
  json += "\"raw\":"      + String(raw)                        + ",";
  json += "\"moisture\":" + String(pct)                        + ",";
  json += "\"pump\":"     + String(relayState ? "true" : "false");
  json += "}";

  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", json);
}

// ── /  — Dashboard with dark theme + auto-reload ─────────────────────────
void handleRoot() {
  int raw = analogRead(SENSOR_PIN);
  int pct = map(raw, 4095, 0, 0, 100);

  String color, stateLabel;
  if      (pct < 25) { color = "#ef5350"; stateLabel = "Dry — pump needed"; }
  else if (pct < 45) { color = "#ffa726"; stateLabel = "Low";               }
  else if (pct < 75) { color = "#66bb6a"; stateLabel = "Adequate";          }
  else               { color = "#42a5f5"; stateLabel = "Saturated";         }

  String pumpClass = relayState ? "badge-on"  : "badge-off";
  String pumpText  = relayState ? "ON"        : "OFF";

  String html = "<!DOCTYPE html><html><head>";
  html += "<meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<meta http-equiv='refresh' content='1'>";
  html += "<title>ESP32 Plant Monitor</title>";
  html += "<style>";
  html += "*{box-sizing:border-box;margin:0;padding:0}";

  // ── Dark theme base ──
  html += "body{font-family:Arial,sans-serif;background:#0f0f0f;color:#e0e0e0}";

  // ── Header ──
  html += "header{background:#1a1a1a;border-bottom:1px solid #2a2a2a;"
          "padding:14px 20px;display:flex;justify-content:space-between;align-items:center}";
  html += "header h1{font-size:16px;font-weight:600;color:#fff}";
  html += ".live{font-size:12px;color:#888;display:flex;align-items:center;gap:6px}";
  html += ".dot{width:8px;height:8px;border-radius:50%;background:#66bb6a}";

  // ── Cards ──
  html += ".cards{display:grid;grid-template-columns:1fr 1fr;gap:12px;padding:16px}";
  html += ".card{background:#1a1a1a;border-radius:12px;padding:16px;"
          "border:1px solid #2a2a2a}";
  html += ".card-label{font-size:12px;color:#666;margin-bottom:6px}";
  html += ".card-value{font-size:30px;font-weight:600;color:#fff}";
  html += ".card-sub{font-size:12px;color:#555;margin-top:4px}";

  // ── Gauge ──
  html += ".gauge-wrap{padding:0 16px 16px}";
  html += ".gauge-label{font-size:12px;color:#666;margin-bottom:6px}";
  html += ".gauge-track{background:#2a2a2a;border-radius:12px;height:26px;overflow:hidden}";
  html += ".gauge-fill{height:100%;border-radius:12px;display:flex;align-items:center;"
          "justify-content:flex-end;padding-right:12px;"
          "font-size:12px;font-weight:600;color:#fff}";
  html += ".ticks{display:flex;justify-content:space-between;"
          "font-size:11px;color:#444;padding:4px 2px 0}";

  // ── Pump card ──
  html += ".pump-card{margin:0 16px 16px;background:#1a1a1a;border-radius:12px;"
          "padding:16px;border:1px solid #2a2a2a;"
          "display:flex;justify-content:space-between;align-items:center}";
  html += ".pump-title{font-size:14px;font-weight:600;color:#fff}";
  html += ".pump-sub{font-size:12px;color:#555;margin-top:3px}";

  // ── Badges ──
  html += ".badge{padding:6px 16px;border-radius:8px;font-size:13px;font-weight:600}";
  html += ".badge-on{background:#1b3a1f;color:#66bb6a;border:1px solid #2e5c33}";
  html += ".badge-off{background:#1f1f1f;color:#555;border:1px solid #2a2a2a}";

  // ── Divider line between sections ──
  html += ".divider{height:1px;background:#1f1f1f;margin:0 16px 16px}";

  // ── Footer ──
  html += "footer{padding:12px 20px;border-top:1px solid #2a2a2a;background:#1a1a1a;"
          "display:flex;justify-content:space-between;font-size:12px;color:#444}";
  html += "footer span:last-child{color:#2e5c33}";

  html += "</style></head><body>";

  // ── Header ──
  html += "<header>"
          "<h1>Plant moisture monitor</h1>"
          "<div class='live'><div class='dot'></div>Auto-reload ON</div>"
          "</header>";

  // ── Metric cards ──
  html += "<div class='cards'>";
  html += "<div class='card'>"
          "<div class='card-label'>Raw ADC value</div>"
          "<div class='card-value'>" + String(raw) + "</div>"
          "<div class='card-sub'>GPIO 32 · 12-bit</div>"
          "</div>";
  html += "<div class='card'>"
          "<div class='card-label'>Moisture</div>"
          "<div class='card-value' style='color:" + color + "'>" + String(pct) + "%</div>"
          "<div class='card-sub' style='color:" + color + ";opacity:0.8'>" + stateLabel + "</div>"
          "</div>";
  html += "</div>";

  html += "<div class='divider'></div>";

  // ── Gauge ──
  html += "<div class='gauge-wrap'>"
          "<div class='gauge-label'>Moisture level</div>"
          "<div class='gauge-track'>"
          "<div class='gauge-fill' style='width:" + String(pct) + "%;background:" + color + "'>"
          + String(pct) + "%"
          "</div></div>"
          "<div class='ticks'>"
          "<span>0%</span><span>25%</span><span>50%</span><span>75%</span><span>100%</span>"
          "</div></div>";

  html += "<div class='divider'></div>";

  // ── Pump state ──
  html += "<div class='pump-card'>"
          "<div>"
          "<div class='pump-title'>Water pump</div>"
          "<div class='pump-sub'>GPIO 5 · threshold ADC &gt; 2000</div>"
          "</div>"
          "<span class='badge " + pumpClass + "'>" + pumpText + "</span>"
          "</div>";

  // ── Footer ──
  html += "<footer>"
          "<span>" + WiFi.localIP().toString() + "</span>"
          "<span>Reloading every 1s</span>"
          "</footer>";

  html += "</body></html>";
  server.send(200, "text/html", html);
}

// ── Setup ─────────────────────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);

  pinMode(CONTROL_PIN, OUTPUT);
  digitalWrite(CONTROL_PIN, LOW);

  WiFi.begin(ssid, password);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected! IP: " + WiFi.localIP().toString());

  server.on("/",     handleRoot);
  server.on("/data", handleData);
  server.begin();
  Serial.println("Server ready — http://" + WiFi.localIP().toString());
}

// ── Loop ──────────────────────────────────────────────────────────────────
void loop() {
  server.handleClient();

  int raw = analogRead(SENSOR_PIN);
  relayState = (raw > 2000);
  digitalWrite(CONTROL_PIN, relayState ? HIGH : LOW);

  delay(500);
}