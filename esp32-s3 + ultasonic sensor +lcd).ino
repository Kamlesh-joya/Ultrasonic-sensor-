#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define TRIG_PIN 4
#define ECHO_PIN 5

// Change LCD address if needed (0x27 or 0x3F)
LiquidCrystal_I2C lcd(0x27, 16, 2);

long duration;
float distance;

void setup() {
  Serial.begin(115200);

  // I2C begin
  Wire.begin(8, 9);   // SDA, SCL

  // LCD setup
  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Ultrasonic");
  lcd.setCursor(0, 1);
  lcd.print("Initializing");
  delay(2000);
  lcd.clear();

  // Ultrasonic pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

void loop() {
  // Trigger pulse
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Read echo
  duration = pulseIn(ECHO_PIN, HIGH, 30000); // 30 ms timeout

  lcd.setCursor(0, 0);
  lcd.print("Distance:");

  lcd.setCursor(0, 1);
  lcd.print("                "); // clear row
  lcd.setCursor(0, 1);

  if (duration == 0) {
    lcd.print("Out of range");
  } else {
    distance = (duration * 0.0343) / 2;

    lcd.print(distance);
    lcd.print(" cm");

    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
  }

  delay(500);
}


