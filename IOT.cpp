
#include <LiquidCrystal.h> // For the LCD screen
#include <Servo.h>         // For the Servo motor

// PIN DEFINITIONS
// Sensors
const int tempPin = A0;
const int ldrPin = A1;
const int soilPin = A2;

// Actuators
const int servoPin = 9;
const int buzzerPin = 8;
const int heaterLEDPin = 7;
const int pumpPin = 10;
const int growLightRelayPin = 6;

// LCD Pins
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
Servo ventServo;

// THRESHOLDS 
const float frostThreshold = 4.0;     // (Celsius) Below this, turn on frost alarm/heater
const float highTempThreshold = 30.0; // (Celsius) Above this, open the vent
const int lowLightThreshold = 300;    // (Analog value) Below this, turn on grow lights
const int drySoilThreshold                                                 = 400;     // (Analog value) Below this, turn on the water pump

void setup() {
  // Initialize Serial Monitor for debugging
  Serial.begin(9600);

  // Initialize LCD
  lcd.begin(16, 2);
  lcd.print("Smart Greenhouse");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");

  // Initialize Servo
  ventServo.attach(servoPin);
  ventServo.write(0); // Start with the vent closed

  // Set pin modes for all output devices
  pinMode(buzzerPin, OUTPUT);
  pinMode(heaterLEDPin, OUTPUT);
  pinMode(pumpPin, OUTPUT);
  pinMode(growLightRelayPin, OUTPUT);

  // Set initial state of outputs to OFF
  digitalWrite(heaterLEDPin, LOW);
  digitalWrite(pumpPin, LOW);
  digitalWrite(growLightRelayPin, LOW); // LOW for relay might mean ON or OFF depending on wiring. Adjust if needed.

  delay(2000); // Wait for 2 seconds
  lcd.clear();
}

void loop() {
  //  1. READ SENSOR DATA 
  
  // Read Temperature (TMP36) and convert to Celsius
  int tempReading = analogRead(tempPin);
  float voltage = tempReading * 5.0 / 1024.0;
  float temperatureC = (voltage - 0.5) * 100;

  // Read Light Level (LDR)
  int lightLevel = analogRead(ldrPin);

  // Read Soil Moisture
  int soilMoisture = analogRead(soilPin);

  // Print values to Serial Monitor for debugging
  Serial.print("Temp: "); Serial.print(temperatureC); Serial.print(" C, ");
  Serial.print("Light: "); Serial.print(lightLevel); Serial.print(", ");
  Serial.print("Soil: "); Serial.println(soilMoisture);

  //  2. IMPLEMENT CONTROL LOGIC 

  // Frost Protection Logic
  if (temperatureC < frostThreshold) {
    digitalWrite(heaterLEDPin, HIGH); // Turn on heater indicator
    digitalWrite(buzzerPin, HIGH);    // Sound the alarm
  } else {
    digitalWrite(heaterLEDPin, LOW); // Turn off heater
    digitalWrite(buzzerPin, LOW);     // Turn off alarm
  }

  // Ventilation Logic
  if (temperatureC > highTempThreshold) {
    ventServo.write(90); // Open vent to 90 degrees
  } else {
    ventServo.write(0); // Close vent
  }

  // Grow Light Logic
  if (lightLevel < lowLightThreshold) {
    digitalWrite(growLightRelayPin, HIGH); // Turn on grow lights
  } else {
    digitalWrite(growLightRelayPin, LOW); // Turn them off
  }

  // Irrigation Logic
  if (soilMoisture < drySoilThreshold) {
    digitalWrite(pumpPin, HIGH); // Turn on water pump
  } else {
    digitalWrite(pumpPin, LOW); // Turn it off
  }

  // --- 3. UPDATE LCD DISPLAY ---
  lcd.clear();
  // Line 1: Temperature and Light
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temperatureC, 1); // Print temp with 1 decimal place
  lcd.print("C L:");
  lcd.print(lightLevel);

  // Line 2: Soil Moisture and System Status
  lcd.setCursor(0, 1);
  lcd.print("Soil:");
  lcd.print(soilMoisture);
  lcd.setCursor(11, 1);
  
  // Display status of actuators
  if (digitalRead(pumpPin) == HIGH) lcd.print("PUMP");
  else if (digitalRead(growLightRelayPin) == HIGH) lcd.print("LITE");
  else if (ventServo.read() > 10) lcd.print("VENT");
  else if (digitalRead(heaterLEDPin) == HIGH) lcd.print("HEAT");
  else lcd.print("OK");


  // Wait for a second before the next loop
  delay(1000);
}