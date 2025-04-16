#include <LiquidCrystal_I2C.h>   // display library
#include <Wire.h>                // I2C library

#define DISPLAY_CHARS 16    // number of characters on a line
#define DISPLAY_LINES 2     // number of display lines
#define DISPLAY_ADDR 0x27   // display address on I2C bus
LiquidCrystal_I2C lcd(DISPLAY_ADDR, DISPLAY_CHARS, DISPLAY_LINES);   // display object


#define LED_GREEN D6
#define LED_RED D7
#define PHOTORESISTOR_IN A0
#define PHOTORESISTOR_THRESHOLD 450
#define BUTTON D3
#define BUTTON_DEBOUNCE_DELAY 20
#define BUZZER D4
#define PHOTOINTERRUPTER D5

int val; //per photointerrupter
unsigned int persone = 0;
unsigned long tempoIn = 0;
unsigned long tempoOut = 0;

void setup() {
  Serial.begin(115200);
  
  Wire.begin();
  Wire.beginTransmission(DISPLAY_ADDR);
  byte error = Wire.endTransmission();
  if (error == 0) {
    Serial.println(F("LCD found."));
    lcd.begin(DISPLAY_CHARS, 2);   // initialize the lcd

  } //else {
  //   Serial.print(F("LCD not found. Error "));
  //   Serial.println(error);
  //   Serial.println(F("Check connections and configuration. Reset to try again!"));
  //   while (true)
  //     delay(1);
  // }
  
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(PHOTOINTERRUPTER,INPUT);
  ledOff();
  // set buzzer pin as outputs
  pinMode(BUZZER, OUTPUT);
  // turn buzzer off
  digitalWrite(BUZZER, HIGH);
  Serial.begin(115200);
  Serial.println(F("\n\nSetup completed.\n\n"));
}

void loop() {
  
  lcd.setBacklight(255);  // set backlight to maximum
  lcd.home();               // move cursor to 0,0
  lcd.clear();
  lcd.print(persone);
  val=digitalRead(PHOTOINTERRUPTER); //read the value of the sensor 
  if(val == HIGH && millis() - tempoOut > 1000 && persone > 0) // turn on LED when sensor is blocked 
	{
		personaOut();
	}
	

  // SE PREMO IL BOTTONE, INTERPRETO COME INGRESSO
  if (isButtonPressed() && millis() - tempoIn > 1000) {
    personaIn();
  }
}

void ledOff() {
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, LOW);
}

void personaIn() {
  tempoIn = millis();
  
  

  if (persone < 5) {
    
    persone++;
    Serial.println(">> INGRESSO");
    Serial.print("Tempo: "); Serial.println(tempoIn);
    Serial.print("Persone: "); Serial.println(persone);
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_GREEN, HIGH);
   
    digitalWrite(BUZZER, LOW);   // turn the buzzer on by making the voltage LOW
    delay(500);                 // wait for a second
    digitalWrite(BUZZER, HIGH);   // turn the buzzer off
    
   }
  if (persone==5){
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
