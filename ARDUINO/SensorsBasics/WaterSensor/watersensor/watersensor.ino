int trig =12;
int echo =13;
int duration;

void setup(){
  Serial.begin(9600); 
 pinmode(trig,OUTPUT);
 pimode(echo,INPUT);

    // Communication started with  9600 baud
}
void loop(){

  int distance,inches,cm;

digitalWrite(trig,LOW);
delayMicroseconds(2);// A short low pulse to clean to ensure a "Clean" high pulse
digitalWrite(trig,HIGH);
delayMicroseconds(10);
digitalWrite(trig,LOW);

distance = pulseIn(echo,HIGH)

inches= MicrosecondToInchest(distance);
cm= MicrosecondToCentimetres(distance);

}

MicrosecondToInchest(long microseconds){ return microsecond/74/2} 
//Sound travels ~74 microseconds per inch.Divide by 2 to get only the distance of the return.
MicrosecondToCentimetres(long microseconds){ return microsecond/29/2}//Same but Sounds travels ~microsecond per centimeter.



