#include <WebServer.h>
#include <WiFi.h>

#define CONTROL_PIN 5  

const char* ssid = "KERALAVISION @vijaya krishnan" ; 
const char* password = "12345678" ; 

WebServer server(80);
 
int raw = 0 ;
int percent = 0 ;

void handleRoot() {
        String html = "<html>\
        <head>\
        <meta http-equiv='refresh' content='1'>\
        </head>\
        <body style='background:linear-gradient(to bottom,#000000,#2f2f2f); color:white; text-align:center;'>\
        <h1> sensor value </h1>\
        <div style = '\
        width:200px;\
        height:200px;\
        border-radius:50%;\
        margin:auto;\
        margin-top:60px;\
        background:conic-gradient(" + String("red ") + String(percent) + "%, #333 0%);\
        '>\
        <h2>"+String(raw)+"</h2>\
        </body>\
        </html>";
        server.send(200,"text/html",html);
             
}         



void setup() {
  pinMode(CONTROL_PIN, OUTPUT);

  Serial.begin(115200);

  WiFi.begin(ssid,password);



while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
}
Serial.println("wifi connected !");
Serial.println(WiFi.localIP());

server.on("/",handleRoot);
server.begin();
}

void loop() {
  raw = analogRead(32);

  if (raw > 2000) {
    digitalWrite(CONTROL_PIN, HIGH); // ON
  } else {
    digitalWrite(CONTROL_PIN, LOW);  // OFF
  }
  server.handleClient();
  percent = map(raw,0,4095,0,100);
  delay(10);
}
