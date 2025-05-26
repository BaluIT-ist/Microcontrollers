#include <Arduino.h>

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
const int flameAnalog = A0;  // AO pin flame
const int flameDigital = 7;  // DO pin flame


long microsecondsToInches(long microseconds);
long microsecondsToCentimeters(long microseconds);
void setcolor(int red, int green, int blue);
void readUltraSonic();
void turnOffAll();
void readFlameSensor();

void setup() {
  Serial.begin(9600);
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
  if (digitalRead(BUTTON0_PIN) == LOW) {
    activeSensor = 0;
    turnOffAll();  // Turn off everything when switching to case 0
    delay(200);
  }
  if (digitalRead(BUTTON1_PIN) == LOW) {
    activeSensor = 1;
    delay(200);
  }
  if (digitalRead(BUTTON2_PIN) == LOW) {
    activeSensor = 2;
    delay(200);
  }

  if (activeSensor == 0) {  
     } else {
    switch (activeSensor) {
      case 1: 
        readUltraSonic(); 
        break;
    }
    switch (activeSensor)
    {
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
  delay(100);
}
void readFlameSensor()
{
int analogValue = analogRead(flameAnalog);  // Read analog (0-1023)
  bool digitalValue = digitalRead(flameDigital);  // Read digital (0 or 1)

  Serial.print("Analog: ");
  Serial.print(analogValue);
  Serial.print(" | Digital: ");
  Serial.println(digitalValue ? "No Flame" : "Flame Detected!");

  delay(500);
}
long microsecondsToInches(long microseconds) {
  return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds) {
  return microseconds / 29 / 2;
}


