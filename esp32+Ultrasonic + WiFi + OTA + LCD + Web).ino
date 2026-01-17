#include <WiFi.h>
#include <ArduinoOTA.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WebServer.h>

/* ========= PIN ========= */
#define SDA_PIN    21
#define SCL_PIN    22
#define TRIG_PIN   27
#define ECHO_PIN   34   // INPUT ONLY pin (safe)

/* ========= LCD ========= */
LiquidCrystal_I2C lcd(0x27, 20, 4);

/* ========= WIFI ========= */
const char* ssid = "Excitel_HARSHIT_2.4G";
const char* password = "dinesh480tak";

/* ========= WEB ========= */
WebServer server(80);

/* ========= VAR ========= */
bool otaEnabled = false;
unsigned long lastMeasure = 0;
float distanceCM = 0;

/* ========= WEB PAGE ========= */
void handleRoot() {
  String html = "<h2>ESP32 PRACTICAL STATUS</h2>";
  html += "<p><b>IP:</b> " + WiFi.localIP().toString() + "</p>";
  html += "<p><b>WiFi:</b> ";
  html += (WiFi.status() == WL_CONNECTED) ? "Connected</p>" : "Disconnected</p>";
  html += "<p><b>OTA:</b> ";
  html += otaEnabled ? "ON</p>" : "OFF</p>";
  html += "<p><b>Practical:</b> ULTRASONIC</p>";
  html += "<p><b>Distance:</b> " + String(distanceCM, 1) + " cm</p>";

  server.send(200, "text/html", html);
}

/* ================= SETUP ================= */
void setup() {
  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(TRIG_PIN, LOW);

  Wire.begin(SDA_PIN, SCL_PIN);
  lcd.init();
  lcd.backlight();

  /* ---- WIFI ---- */
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);
  WiFi.begin(ssid, password);

  lcd.setCursor(0,0);
  lcd.print("Connecting WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
  }

  /* ---- OTA ---- */
  ArduinoOTA.setHostname("ESP32-ULTRA");
  ArduinoOTA.begin();
  otaEnabled = true;

  /* ---- WEB ---- */
  server.on("/", handleRoot);
  server.begin();
}

/* ================= LOOP ================= */
void loop() {
  ArduinoOTA.handle();
  server.handleClient();

  measureDistance();
  updateLCD();
}

/* ================= ULTRASONIC ================= */
void measureDistance() {
  if (millis() - lastMeasure < 1000) return;
  lastMeasure = millis();

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // 30ms timeout

  if (duration > 0) {
    distanceCM = (duration * 0.034) / 2;
  }
}

/* ================= LCD UPDATE ================= */
void updateLCD() {
  static unsigned long lastLCD = 0;
  if (millis() - lastLCD < 1000) return;
  lastLCD = millis();

  lcd.clear();

  // Row 1
  lcd.setCursor(0,0);
  lcd.print("IP:");
  lcd.print(WiFi.localIP());

  // Row 2
  lcd.setCursor(0,1);
  lcd.print("OTA:");
  lcd.print(otaEnabled ? "ON " : "OFF");
  lcd.print(" WiFi:");
  lcd.print(WiFi.status() == WL_CONNECTED ? "ON" : "OFF");

  // Row 3
  lcd.setCursor(0,2);
  lcd.print("Practical:ULTRA");

  // Row 4
  lcd.setCursor(0,3);
  lcd.print("Sensor:");
  lcd.print(distanceCM, 1);
  lcd.print(" cm");
}


