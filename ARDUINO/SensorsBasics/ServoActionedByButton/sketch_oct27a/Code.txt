#include <Servo.h>


Servo myservo;
int buttonPin = 10;
bool servoActive=false;

void setup() {
  Serial.begin(9600);
  myservo.attach(9);
  pinMode(buttonPin,INPUT_PULLUP);
    
}

void loop() {
 
    if(digitalRead(buttonPin)==LOW){

      if(!servoActive){
        myservo.write(90);
        servoActive=true;
      }
      else
      {
        if(servoActive){
           myservo.write(0);
        servoActive=false;
        }
      }
  }

delay(100);
  
  
}
  
