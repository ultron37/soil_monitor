#define INPUT_PIN 7
#define RELAY_PIN 8

void setup() {
  pinMode(INPUT_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);

  digitalWrite(RELAY_PIN, HIGH); // OFF
}

void loop() {
  int state = digitalRead(INPUT_PIN);

  if (state == HIGH) {
    digitalWrite(RELAY_PIN, LOW);  // ON
  } else {
    digitalWrite(RELAY_PIN, HIGH); // OFF
  }
}