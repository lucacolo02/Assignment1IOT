#include <LiquidCrystal_I2C.h>   // display library
#include <Wire.h>                // I2C library
#include <ESP8266WiFi.h>
#include "secrets.h"
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include <InfluxDbClient.h>
#include <ESP8266WebServer.h>
// InfluxDB cfg
InfluxDBClient client_idb(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN);
Point pointDevice("GioelleriaStatus");
Point pointPersone("PersoneInStatus");
// WiFi cfg
char ssid[] = SECRET_SSID;   // your network SSID (name)
char pass[] = SECRET_PASS;   // your network password
#ifdef IP
IPAddress ip(IP);
IPAddress subnet(SUBNET);
IPAddress dns(DNS);
IPAddress gateway(GATEWAY);
#endif
WiFiClient client;

// MySQL server cfg
char mysql_user[] = MYSQL_USER;       // MySQL user login username
char mysql_password[] = MYSQL_PASS;   // MySQL user login password
IPAddress server_addr(MYSQL_IP);      // IP of the MySQL *server* here
MySQL_Connection conn((Client *)&client);

char query[128];
char INSERT_DATA[] = "INSERT INTO `%s`.`Gioielleria` (`led_status`,`Temperature`) VALUES ('%s','%f')";


#define DISPLAY_CHARS 16    // number of characters on a line
#define DISPLAY_LINES 2     // number of display lines
#define DISPLAY_ADDR 0x27   // display address on I2C bus
LiquidCrystal_I2C lcd(DISPLAY_ADDR, DISPLAY_CHARS, DISPLAY_LINES);   // display object

#define NTC_PIN A0     // NTC analog pin
#define NTC_R1 10000   // value of R1 on the module (10KOhm)

// Steinhart-Hart coefficients for the NTC
#define NTC_A 3.354016e-03
#define NTC_B 2.569850e-04
#define NTC_C 2.620131e-06
#define NTC_D 6.383091e-08
#define LED_GREEN D6
#define LED_RED D7
#define PHOTORESISTOR_IN A0
#define PHOTORESISTOR_THRESHOLD 450
#define BUTTON D3
#define BUTTON_DEBOUNCE_DELAY 20
#define BUZZER D4
#define PHOTOINTERRUPTER D5
#define LED_ONBOARD LED_BUILTIN_AUX 

ESP8266WebServer server(80);   // HTTP server on port 80

bool led_state = HIGH;

int val; //per photointerrupter
unsigned int persone = 0;
unsigned long tempoIn = 0;
unsigned long tempoOut = 0;
char led_status[20];
long rssi;
float T;
bool acceso;

void setup() {
  WiFi.mode(WIFI_STA);
  server.on("/", handle_root);
  server.on("/ON", handle_ledon);
  server.on("/OFF", handle_ledoff);
  server.onNotFound(handle_NotFound);
  server.begin();
  Serial.begin(115200);
  
  Wire.begin();
  Wire.beginTransmission(DISPLAY_ADDR);
  byte error = Wire.endTransmission();
  if (error == 0) {
    Serial.println(F("LCD found."));
    lcd.begin(DISPLAY_CHARS, 2);   // initialize the lcd
    acceso=1;
  }
  pinMode(LED_ONBOARD, OUTPUT);
  digitalWrite(LED_ONBOARD, HIGH);
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
  aggiornaDisplay(persone);
}

void loop() {
  int static init_db = 0;
  rssi = connectToWiFi();  
  // lcd.setBacklight(255);  // set backlight to maximum
  // lcd.home();               // move cursor to 0,0
  // lcd.clear();
  // lcd.print(persone);
  val=digitalRead(PHOTOINTERRUPTER); //read the value of the sensor 
  if(acceso==0 && led_state==HIGH){
    lcd.begin(DISPLAY_CHARS, 2);   // initialize the lcd
    acceso=1;
    aggiornaDisplay(persone);
    WriteMultiToInflux(persone, T, rssi, led_state);   // write on InfluxDB
  }
  if(led_state==LOW && acceso==1)
  {
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_GREEN, LOW);
    persone=0;
    lcd.noDisplay();
    acceso=0;
    WriteMultiToInflux(persone, T, rssi, led_state);   // write on InfluxDB

  }
  if(val == HIGH && millis() - tempoOut > 1000 && persone > 0 && led_state==HIGH) // turn on LED when sensor is blocked 
	{
		personaOut();
    aggiornaDisplay(persone);
	}
	

  // SE PREMO IL BOTTONE, INTERPRETO COME INGRESSO
  if (isButtonPressed() && millis() - tempoIn > 1000 && persone<5 && led_state==HIGH) {
    personaIn();
    aggiornaDisplay(persone);
  }
  
  // if (init_db == 0) {   // set tags
  //   pointDevice.addTag("device", "ESP8266");
  //   pointDevice.addTag("SSID", WiFi.SSID());
    
  //   init_db = 1;
  // }
  server.handleClient();   // listening for clients on port 80
  digitalWrite(LED_ONBOARD, !led_state);
}

void ledOff() {
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, LOW);
}

void personaIn() {
  tempoIn = millis();
  T=CalcolaTemp();
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
    strcpy(led_status, "Accesso consentito");
    

    
   }
  if (persone==5){
    digitalWrite(LED_RED, HIGH);  // ROSSO alla 5ª persona
    digitalWrite(LED_GREEN, LOW);
    strcpy(led_status, "Massima capienza");
   

  }
 WriteMultiToDB(led_status, T);   // write on MySQL table if connection works
 WriteMultiToInflux(persone, T, rssi, led_state);   // write on InfluxDB
 WritePersoneToInflux(persone);
}

void personaOut() {
  tempoOut = millis();
  persone--;
  T=CalcolaTemp();

  Serial.println("<< USCITA");
  Serial.print("Tempo: "); Serial.println(tempoOut);
  Serial.print("Persone: "); Serial.println(persone);

  if (persone < 5) {
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_GREEN, HIGH);
  }
  WriteMultiToInflux(persone, T, rssi, led_state);   // write on InfluxDB
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

int WriteMultiToDB(char led_status[], float T) {

  // connect to MySQL
  if (!conn.connected()) {
    conn.close();
    Serial.println(F("Connecting to MySQL..."));
    if (conn.connect(server_addr, 3306, mysql_user, mysql_password)) {
      Serial.println(F("MySQL connection established."));
    } else {
      Serial.println(F("MySQL connection failed."));
      return -1;
    }
  }

  // log data
  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
  sprintf(query, INSERT_DATA, mysql_user, led_status, T);
  Serial.println(query);
  // execute the query
  cur_mem->execute(query);
  // Note: since there are no results, we do not need to read any data
  // deleting the cursor also frees up memory used
  delete cur_mem;
  Serial.println(F("Data recorded on MySQL"));

  return 1;
}
long connectToWiFi() {
  long rssi_strength;
  // connect to WiFi (if not already connected)
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print(F("Connecting to SSID: "));
    Serial.println(ssid);

#ifdef IP
    WiFi.config(ip, dns, gateway, subnet);   // by default network is configured using DHCP
#endif

    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) {
      Serial.print(F("."));
      delay(250);
    }
    Serial.println(F("\nConnected!"));
    rssi_strength = WiFi.RSSI();   // get wifi signal strength
    printWifiStatus();
    
  } else {
    rssi_strength = WiFi.RSSI();   // get wifi signal strength
  }

  return rssi_strength;
}
void WriteMultiToInflux(int persone, float T, long rssi, bool led_state) {

  // store measured value into point
  pointDevice.clearFields();
  pointPersone.clearTags();
  pointDevice.addField("persone", persone);
  pointDevice.addField("temperatura", T);
  pointDevice.addField("rssi", rssi);
  pointDevice.addField("led_state", led_state);

  Serial.print(F("Writing: "));
  Serial.println(pointDevice.toLineProtocol());
  if (!client_idb.writePoint(pointDevice)) {
    Serial.print(F("InfluxDB write failed: "));
    Serial.println(client_idb.getLastErrorMessage());
  }
}
void WritePersoneToInflux(int persone) {
  pointPersone.clearFields();
  pointPersone.clearTags();

  pointPersone.addField("persone_attuali", persone);

  Serial.print(F("Writing ingresso: "));
  Serial.println(pointPersone.toLineProtocol());

  if (!client_idb.writePoint(pointPersone)) {
    Serial.print(F("InfluxDB write failed: "));
    Serial.println(client_idb.getLastErrorMessage());
  }
}

void check_influxdb() {
  // check InfluxDB server connection
  if (client_idb.validateConnection()) {
    Serial.print(F("Connected to InfluxDB: "));
    Serial.println(client_idb.getServerUrl());
  } else {
    Serial.print(F("InfluxDB connection failed: "));
    Serial.println(client_idb.getLastErrorMessage());
  }
}
float CalcolaTemp(){
  float Vo = analogRead(NTC_PIN);              // voltage, range 0~1023
  float R2 = NTC_R1 * ((float)Vo / 1023.0f);   // compute the resistance on thermistor at current temperature
  float logR2R1 = log(R2 / NTC_R1);
  float T = 1.0f / (NTC_A + (NTC_B * logR2R1) + (NTC_C * (logR2R1 * logR2R1)) + (NTC_D * (logR2R1 * logR2R1 * logR2R1)));   // temperature in Kelvin
  T = T - 273.15f;
  return T;
}
void handle_root() {
  Serial.print(F("New Client with IP: "));
  Serial.println(server.client().remoteIP().toString());
  server.send(200, F("text/html"), SendHTML(led_state));
}

void handle_ledon() {
  led_state = HIGH;
  Serial.println(F("Led ON"));
  server.send(200, F("text/html"), SendHTML(led_state));
}

void handle_ledoff() {
  led_state = LOW;
  Serial.println(F("Led OFF"));
  server.send(200, F("text/html"), SendHTML(led_state));
}

void handle_NotFound() {
  server.send(404, F("text/plain"), F("Not found"));
}
String SendHTML(uint8_t ledstat) {
  String ptr = F(
    "<!DOCTYPE html><html lang=\"en\">\n"
    "<head>\n"
    "<meta charset=\"UTF-8\">\n"
    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
    "<meta http-equiv=\"refresh\" content=\"30\">\n"
    "<title>Web LED Control</title>\n"
    "<link rel=\"icon\" href=\"data:image/svg+xml,<svg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 24 24'><path fill='limegreen' d='M12 2a10 10 0 1 0 10 10A10.011 10.011 0 0 0 12 2zm1 14h-2v-2h2zm0-4h-2V7h2z'/></svg>\">\n"
    "<style>\n"
    "body { font-family: 'Segoe UI', sans-serif; background: #f0f0f0; margin: 0; padding: 0; text-align: center; }\n"
    "h1 { color: #2c3e50; margin-top: 40px; }\n"
    "h3 { color: #34495e; }\n"
    ".status { font-size: 18px; margin: 20px auto; color: #555; }\n"
    ".led-on { color: #27ae60; font-weight: bold; }\n"
    ".led-off { color: #c0392b; font-weight: bold; }\n"
    ".button { display: inline-block; padding: 15px 35px; font-size: 20px; border: none; border-radius: 8px; text-decoration: none; color: white; transition: background 0.3s ease; margin-top: 20px; }\n"
    ".button-on { background-color: #27ae60; }\n"
    ".button-on:hover { background-color: #219150; }\n"
    ".button-off { background-color: #e74c3c; }\n"
    ".button-off:hover { background-color: #c0392b; }\n"
    "footer { margin-top: 40px; font-size: 12px; color: #aaa; }\n"
    "</style>\n"
    "</head>\n"
    "<body>\n"
    "<h1>🔌 Gioielleria </h1>\n");

  if (ledstat) {
    ptr += F("<p class=\"status\">Stato del sistema attuale: <span class=\"led-on\">ON</span></p>\n");
    ptr += F("<a class=\"button button-off\" href=\"/OFF\">Chiudi Negozio</a>\n");
  } else {
    ptr += F("<p class=\"status\">Stato del sistema attuale: <span class=\"led-off\">OFF</span></p>\n");
    ptr += F("<a class=\"button button-on\" href=\"/ON\">Apri Negozio</a>\n");
  }

  ptr += F(
    "<footer>ESP32 Web Controller &copy; 2025</footer>\n"
    "</body>\n"
    "</html>\n");

  return ptr;
}

void aggiornaDisplay(int persone) {
  lcd.clear();       
  lcd.setBacklight(255);  
  lcd.home();           // Pulisce il display
  lcd.setCursor(0, 0);            // Riga 0, colonna 0
  lcd.print("Persone: ");
  lcd.print(persone);
  lcd.setCursor(0, 1);            // Riga 1, colonna 0

  if (persone < 5) {
    lcd.print("Accessibile");
  } else {
    lcd.print("Massima capienza");
  }
}

void printWifiStatus() {
  Serial.println(F("\n=== WiFi connection status ==="));

  // SSID
  Serial.print(F("SSID: "));
  Serial.println(WiFi.SSID());

  // signal strength
  Serial.print(F("Signal strength (RSSI): "));
  Serial.print(WiFi.RSSI());
  Serial.println(F(" dBm"));

  // current IP
  Serial.print(F("IP Address: "));
  Serial.println(WiFi.localIP().toString());
  // WiFi.localIP().printTo(Serial);

  // subnet mask
  Serial.print(F("Subnet mask: "));
  Serial.println(WiFi.subnetMask().toString());

  // gateway
  Serial.print(F("Gateway IP: "));
  Serial.println(WiFi.gatewayIP().toString());

  // DNS
  Serial.print(F("DNS IP: "));
  Serial.println(WiFi.dnsIP().toString());

  Serial.println(F("==============================\n"));
}
