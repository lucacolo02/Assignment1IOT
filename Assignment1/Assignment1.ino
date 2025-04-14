#define LED_GREEN D1
#define LED_RED D2
#define PHOTORESISTOR_IN A0
#define PHOTORESISTOR_THRESHOLD 450
#define BUTTON D3
#define BUTTON_DEBOUNCE_DELAY 20
#define BUZZER D4

unsigned int persone = 0;
unsigned long tempoIn = 0;
unsigned long tempoOut = 0;

void setup() {
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);
  ledOff();
  // set buzzer pin as outputs
  pinMode(BUZZER, OUTPUT);
  // turn buzzer off
  digitalWrite(BUZZER, HIGH);
  Serial.begin(115200);
  Serial.println(F("\n\nSetup completed.\n\n"));
}

void loop() {
  unsigned int lightSensorValueIn = analogRead(PHOTORESISTOR_IN);

  // SE IL VALORE È BASSO, INTERPRETIAMO COME USCITA
  if (lightSensorValueIn <= PHOTORESISTOR_THRESHOLD && persone > 0 && millis() - tempoOut > 1000) {
    personaOut();
  }

  // SE PREMO IL BOTTONE, INTERPRETO COME INGRESSO
  if (isButtonPressed() && persone < 5 && millis() - tempoIn > 1000) {
    personaIn();
  }
}

void ledOff() {
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, LOW);
}

void personaIn() {
  tempoIn = millis();
  persone++;
  Serial.println(">> INGRESSO");
  Serial.print("Tempo: "); Serial.println(tempoIn);
  Serial.print("Persone: "); Serial.println(persone);

  if (persone < 5) {
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_GREEN, HIGH);
      Serial.println(F("Buzzer: ON"));
  digitalWrite(BUZZER, LOW);   // turn the buzzer on by making the voltage LOW
  delay(1000);                 // wait for a second
  Serial.println(F("Buzzer: OFF"));
  digitalWrite(BUZZER, HIGH);   // turn the buzzer off
  } else {
    digitalWrite(LED_RED, HIGH);  // ROSSO alla 5ª persona
    digitalWrite(LED_GREEN, LOW);
  }
}

void personaOut() {
  tempoOut = millis();
  persone--;

  Serial.println("<< USCITA");
  Serial.print("Tempo: "); Serial.println(tempoOut);
  Serial.print("Persone: "); Serial.println(persone);

  if (persone < 5) {
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_GREEN, HIGH);
  }
}

bool isButtonPressed() {
  static byte lastButtonState = HIGH;
  static byte buttonState = HIGH;
  static unsigned long lastDebounceTime = 0;

  byte reading = digitalRead(BUTTON);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > BUTTON_DEBOUNCE_DELAY) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == LOW) {
        return true;
      }
    }
  }

  lastButtonState = reading;
  return false;
}
