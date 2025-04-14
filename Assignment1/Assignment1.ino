#define LED_GREEN D1
#define LED_RED D2
#define PHOTORESISTOR_IN A0
#define PHOTORESISTOR_THRESHOLD 450
#define BUTTON D3
#define BUTTON_DEBOUNCE_DELAY 20

unsigned int persone = 0;
unsigned long tempoIn = 0;
unsigned long tempoOut = 0;

void setup() {
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);
  ledOff();

  Serial.begin(115200);
  Serial.println(F("\n\nSetup completed.\n\n"));
}

void loop() {
  unsigned int lightSensorValueIn = analogRead(PHOTORESISTOR_IN);
  personaIn(lightSensorValueIn);

  if (isButtonPressed() && persone > 0 && (millis() - tempoOut > 1000)) {
    personaOut();
  }
}

void ledOff() {
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, LOW);
}

void personaIn(int lightSensorValueIn) {
  if (lightSensorValueIn <= PHOTORESISTOR_THRESHOLD) {
    if (millis() - tempoIn > 1000) {
      tempoIn = millis();
      if (persone < 5) {
        persone++;
        Serial.println(">> INGRESSO");
        Serial.print("Tempo: "); Serial.println(tempoIn);
        Serial.print("Persone: "); Serial.println(persone);
        digitalWrite(LED_RED, LOW);
        digitalWrite(LED_GREEN, HIGH);
        if (persone==5)
        {
        digitalWrite(LED_RED, HIGH);
        digitalWrite(LED_GREEN, LOW);
        }
      } else  {
        digitalWrite(LED_RED, HIGH);
        digitalWrite(LED_GREEN, LOW);
      }
    }
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

void personaOut() {
  tempoOut = millis();
  persone--;
  Serial.println("<< USCITA");
  Serial.print("Tempo: "); Serial.println(tempoOut);
  Serial.print("Persone: "); Serial.println(persone);
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, HIGH);
}

