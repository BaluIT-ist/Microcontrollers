#include <Arduino.h>             // Includes the core Arduino functions
#include <Wire.h>                // Includes I2C communication library
#include <Adafruit_GFX.h>        // Graphics library for OLED and displays
#include <Adafruit_SSD1306.h>    // Library for SSD1306 OLED display

#define SCREEN_WIDTH 128         // OLED display width in pixels
#define SCREEN_HEIGHT 64         // OLED display height in pixels
#define OLED_RESET -1            // OLED reset pin (not used here)

// Create display object
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// --- PIN DEFINITIONS ---
int echo = 9;                    // Echo pin for ultrasonic sensor
int trig = 8;                    // Trigger pin for ultrasonic sensor
int buzzer = 2;                  // Buzzer pin for alerts
int bluePin = 5;                 // RGB LED blue pin
int greenPin = 6;                // RGB LED green pin
int redPin = 3;                  // RGB LED red pin

int activeSensor = 0;           // 0 = Menu, 1 = Ultrasonic, 2 = Flame Sensor

const int BUTTON0_PIN = 10;     // Button to select Menu
const int BUTTON1_PIN = 11;     // Button to select Ultrasonic mode
const int BUTTON2_PIN = 12;     // Button to select Flame Sensor mode

const int flameAnalog = A0;     // Flame sensor analog output pin
const int flameDigital = 7;     // Flame sensor digital output pin

// --- FUNCTION PROTOTYPES ---
long microsecondsToInches(long microseconds);               // Converts microseconds to inches
long microsecondsToCentimeters(long microseconds);          // Converts microseconds to centimeters
void setcolor(int red, int green, int blue);                // Sets RGB LED color
void readUltraSonic();                                      // Handles ultrasonic sensor logic
void turnOffAll();                                          // Turns off buzzer and RGB LED
void readFlameSensor();                                     // Handles flame sensor logic
void displayMenuScreen();                                   // Displays sensor selection screen
void displayDistance(long cm, long inches);                 // Displays distance data on OLED
void displayFlameData(int analogValue, bool digitalValue);  // Displays flame sensor data

// --- SETUP FUNCTION ---
void setup() {
  Serial.begin(9600); // Starts serial communication

  // Initialize OLED display //F() macro for storing the string in flash memore(program memory) instead of  RAM
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Infinite loop if display init fails
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println(F("System Starting"));
  display.display();
  delay(2000);

  // Setup pin modes
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

// --- MAIN LOOP FUNCTION ---
void loop() {
  
  if (digitalRead(BUTTON0_PIN) == LOW && digitalRead(BUTTON1_PIN) == LOW && digitalRead(BUTTON2_PIN) == LOW) {
    setcolor(0, 0, 255); 
    delay(3000);
    return;


  }

  // Check for button press to switch sensor mode
  if (digitalRead(BUTTON0_PIN) == LOW) {
    activeSensor = 0;
    turnOffAll();
    delay(200); // Debounce delay
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
 

  // Call appropriate function based on selected mode
  if (activeSensor == 0) {
    displayMenuScreen(); // Display mode selection
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

// --- HELPER FUNCTIONS ---

// Turns off buzzer and sets RGB LED to off
void turnOffAll() {
  setcolor(0, 0, 0);
  noTone(buzzer);
}

// Sets RGB LED to given red, green, blue values
void setcolor(int red, int green, int blue) {
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);
}

// Reads and processes ultrasonic distance data
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

  // Alert and color indication based on distance
  if (cm <= 10) {
    setcolor(255, 0, 0); tone(buzzer, 1000);      // Red & Loud
  }
  else if (cm <= 20) {
    setcolor(255, 100, 0); tone(buzzer, 800);     // Orange
  }
  else if (cm <= 30) {
    setcolor(255, 255, 0); tone(buzzer, 600);     // Yellow
  }
  else if (cm <= 40) {
    setcolor(0, 255, 0); tone(buzzer, 400);       // Green
  }
  else if (cm <= 50) {
    setcolor(0, 0, 255); tone(buzzer, 200);       // Blue
  }
  else {
    turnOffAll(); // No alert for distant objects
  }

  displayDistance(cm, inches); // Show on OLED
  delay(100);
}

// Reads flame sensor and alerts if fire is detected
void readFlameSensor() {
  if (activeSensor != 2) return;

  int analogValue = analogRead(flameAnalog);
  bool digitalValue = digitalRead(flameDigital);

  static unsigned long lastToggleTime = 0;
  static bool alertState = false;

  // Fire detection
  if (!digitalValue) { // Fire detected
    if (millis() - lastToggleTime > 500) {
      alertState = !alertState;
      lastToggleTime = millis();
      
      if (alertState) {
        setcolor(255, 0, 0); // Red ON
        tone(buzzer, 1000);  // Alarm
      } else {
        setcolor(0, 0, 0); // OFF
        noTone(buzzer);
      }
    }
  } else {
    setcolor(0, 255, 0); // Safe = Green
    noTone(buzzer);
  }

  // Show status on OLED
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(F("FLAME SENSOR"));
  display.print(F("Status: "));
  display.println(digitalValue ? F("SAFE") : F("FIRE DETECTED!"));

  if (!digitalValue && alertState) {
    display.setCursor(0, 30);
    display.println(F("EVACUATE AREA!"));
  }
  display.display();
}

// Displays the sensor selection menu
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

// Displays distance in cm/inch with warnings
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

// Displays flame sensor values (if you use this)
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

// Converts time to inches
long microsecondsToInches(long microseconds) {
  return microseconds / 74 / 2;  //~74 microseconds per inch 
}

// Converts time to centimeters
long microsecondsToCentimeters(long microseconds) {
  return microseconds / 29 / 2; //~29 microseconds per cm
}


