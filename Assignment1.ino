#define LED_GREEN D1   // green LED pin
#define LED_RED D2 //red LED pin
#define PHOTORESISTOR A0              // photoresistor pin
#define PHOTORESISTOR_THRESHOLD 450   // turn led on for light values lesser than this
//#define PERSONE 0  //numero persone all'interno del negozio
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
  static unsigned int lightSensorValue;
  static unsigned int persone;
  static unsigned long tempo = 0;
  
  lightSensorValue = analogRead(PHOTORESISTOR);   // read analog value (range 0-1023)


  if (lightSensorValue <= PHOTORESISTOR_THRESHOLD) {   // high brightness
    if(millis()-tempo>2000){
      tempo=millis();
      persone=persone+1;
      if (persone<=5){
        digitalWrite(LED_RED, LOW);                               
        digitalWrite(LED_GREEN, HIGH);
      }else{
        digitalWrite(LED_RED, HIGH);
        digitalWrite(LED_GREEN, LOW);  
      }                          
  }    
    }
       
    
}


void ledOff() {
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, LOW);
}
