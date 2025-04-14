#define LED_GREEN D1   // green LED pin
#define LED_RED D2 //red LED pin
#define PHOTORESISTOR_IN A0              // photoresistor in
#define PHOTORESISTOR_OUT D3  //photoresistor out
#define PHOTORESISTOR_THRESHOLD 450   // turn led on for light values lesser than this
unsigned int persone = 0;
unsigned long tempo = 0;
void setup() {
  // set LED pin as outputs
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);

  
  // turn led off
  ledOff();

  Serial.begin(115200);
  Serial.println(F("\n\nSetup completed.\n\n"));
}

void loop() {
  static unsigned int lightSensorValueIn;
  
  lightSensorValueIn = analogRead(PHOTORESISTOR_IN);   // read analog value (range 0-1023)
  //lightSensorValueOut = analogRead(PHOTORESISTOR_OUT);   // read analog value (range 0-1023)

  personaIn(lightSensorValueIn);

  
       
    
}


void ledOff() {
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, LOW);
}

void personaIn(int lightSensorValueIn ){
if (lightSensorValueIn <= PHOTORESISTOR_THRESHOLD) {   // high brightness
    if(millis()-tempo>1000){
      tempo=millis();
      Serial.print("Tempo: ");
      Serial.println(tempo);
      Serial.print("Persone: ");
      Serial.println(persone);
      if (persone<=5){
        persone=persone+1;
        digitalWrite(LED_RED, LOW);                               
        digitalWrite(LED_GREEN, HIGH);
      }else{
        digitalWrite(LED_RED, HIGH);
        digitalWrite(LED_GREEN, LOW);  
      }                          
  }    
    }
}
