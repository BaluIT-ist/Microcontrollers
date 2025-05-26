int trig = 12;
int echo = 13;

void setup() {
  Serial.begin(9600); 
  pinMode(trig, OUTPUT);  // Corrected from 'pinmode' to 'pinMode'
  pinMode(echo, INPUT);   // Corrected from 'pimode' to 'pinMode'
}

void loop() {
  long duration, inches, cm;

  digitalWrite(trig, LOW);
  delayMicroseconds(2);  // Clear trigger
  digitalWrite(trig, HIGH);
  delayMicroseconds(10); // 10µs pulse
  digitalWrite(trig, LOW);

  duration = pulseIn(echo, HIGH); // Measure echo time (microseconds)

  inches = microsecondsToInches(duration);
  cm = microsecondsToCentimeters(duration); 

  Serial.print("Distance: ");
  Serial.print(inches);
  Serial.print(" in, ");
  Serial.print(cm);
  Serial.println(" cm");

  delay(100); // Delay between readings
}

// Helper functions (corrected)
long microsecondsToInches(long microseconds) {
  return microseconds / 74 / 2;  //Sound travels ~74 microseconds per inch.Divide by 2 to get only the distance of the return.
}

long microsecondsToCentimeters(long microseconds) {
  return microseconds / 29 / 2;  // Same but Sounds travels ~ 29(or  precisely 29.1)microsecond per centimeter
}