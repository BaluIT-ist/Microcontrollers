#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED Display Configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Pin Definitions
int echo = 9;
int trig = 8;
int buzzer = 2;
int bluePin = 5;
int greenPin = 6;
int redPin = 3; 
int activeSensor = 0;
const int BUTTON0_PIN = 10;
const int BUTTON1_PIN = 11;
const int BUTTON2_PIN = 12;
const int flameAnalog = A0;
const int flameDigital = 7;

// Function Prototypes
long microsecondsToInches(long microseconds);
long microsecondsToCentimeters(long microseconds);
void setcolor(int red, int green, int blue);
void readUltraSonic();
void turnOffAll();
void readFlameSensor();
void displayMenuScreen();
void displayDistance(long cm, long inches);
void displayFlameData(int analogValue, bool digitalValue);

void setup() {
  Serial.begin(9600);
  
  // Initialize OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println(F("System Starting"));
  display.display();
  delay(2000);

  // Pin Modes
  pinMode(trig, OUTPUT); 
  pinMode(echo, INPUT);  
  pinMode(buzzer, OUTPUT);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(BUTTON0_PIN, INPUT_PULLUP);
  pinMode(BUTTON1_PIN, INPUT_PULLUP);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);
  pinMode(flameDigital, INPUT);
}

void loop() {
  // Button Handling
  if (digitalRead(BUTTON0_PIN) == LOW) {
    activeSensor = 0;
    turnOffAll();
    delay(200);
  }
  if (digitalRead(BUTTON1_PIN) == LOW) {
    activeSensor = 1;
    turnOffAll();
    delay(200);
  }
  if (digitalRead(BUTTON2_PIN) == LOW) {
    activeSensor = 2;
    turnOffAll();
    delay(200);
  }

  // Mode Selection
  if (activeSensor == 0) {
    displayMenuScreen();
  } else {
    switch (activeSensor) {
      case 1: 
        readUltraSonic(); 
        break;
      case 2:
        readFlameSensor();
        break;
    }
  }
}

void turnOffAll() {
  setcolor(0, 0, 0);
  noTone(buzzer);
}

void setcolor(int red, int green, int blue) {
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);
}

void readUltraSonic() {
  if (activeSensor != 1) {
    turnOffAll();
    return;
  }

  long duration, inches, cm;

  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  duration = pulseIn(echo, HIGH);
  inches = microsecondsToInches(duration);
  cm = microsecondsToCentimeters(duration);

  Serial.print("Distance: ");
  Serial.print(inches);
  Serial.print(" inch, ");
  Serial.print(cm);
  Serial.println(" cm");

  // Visual and audio feedback
  if (cm <= 10) {
    setcolor(255, 0, 0);
    tone(buzzer, 1000);
  }
  else if (cm <= 20) {
    setcolor(255, 100, 0);
    tone(buzzer, 800);
  }
  else if (cm <= 30) {
    setcolor(255, 255, 0);
    tone(buzzer, 600);
  }
  else if (cm <= 40) {
    setcolor(0, 255, 0);
    tone(buzzer, 400);
  }
  else if (cm <= 50) {
    setcolor(0, 0, 255);
    tone(buzzer, 200);
  }
  else {
    turnOffAll();
  }

  displayDistance(cm, inches);
  delay(100);
}

void readFlameSensor() {
  if (activeSensor != 2) return; // Only run in flame sensor mode

  int analogValue = analogRead(flameAnalog);
  bool digitalValue = digitalRead(flameDigital);
  static unsigned long lastToggleTime = 0;
  static bool alertState = false; // Tracks LED/buzzer state

  // Flame detection logic
  if (!digitalValue) { // Fire detected!
    if (millis() - lastToggleTime > 500) { // Toggle every 500ms
      alertState = !alertState;
      lastToggleTime = millis();
      
      if (alertState) {
        setcolor(255, 0, 0); // Red LED ON
        tone(buzzer, 1000);  // Buzzer ON (1kHz)
      } else {
        setcolor(0, 0, 0);    // LED OFF
        noTone(buzzer);       // Buzzer OFF
      }
    }
  } else {
    setcolor(0, 255, 0); // Green = No fire
    noTone(buzzer);
  }

  // OLED Display (with blinking warning)
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(F("FLAME SENSOR"));
  display.print(F("Status: "));
  display.println(digitalValue ? F("SAFE") : F("FIRE DETECTED!"));
  
  if (!digitalValue) {
    // Blinking warning text
    if (alertState) {
      display.setCursor(0, 30);
      display.println(F("EVACUATE AREA!"));
    }
  }
  display.display();
}

// OLED Display Functions
void displayMenuScreen() {
  display.clearDisplay();
  display.setTextSize(1); 
  display.setTextColor(SSD1306_WHITE);  
  display.setCursor(0,0);  
  display.println(F("Select sensor:"));
  display.println(F("1: Ultrasonic"));
  display.println(F("2: Flame Sensor"));
  display.display();
}

void displayDistance(long cm, long inches) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print(F("Distance: "));
  display.print(cm);
  display.println(F(" cm"));
  display.print(inches);
  display.println(F(" inches"));
  
  if(cm <= 50) {
    display.setCursor(0,30);
    display.print(F("WARNING!"));
    if(cm <= 10) display.print(F(" TOO CLOSE!"));
  }
  display.display();
}

void displayFlameData(int analogValue, bool digitalValue) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.println(F("Flame Sensor"));
  display.print(F("Analog: "));
  display.println(analogValue);
  display.print(F("Digital: "));
  display.println(digitalValue ? F("No Flame") : F("FLAME DETECTED!"));
  
  if(!digitalValue) {
    display.setCursor(0,40);
    display.println(F("WARNING! FIRE!"));
  }
  display.display();
}

// Conversion Functions
long microsecondsToInches(long microseconds) {
  return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds) {
  return microseconds / 29 / 2;
}