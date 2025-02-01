#include <Servo.h>

Servo Clamp; //creating a servo object called Clamp

int servoPin = 5; //outputting values the Servo Clamp
int angle = 0;

void setup() {
  // put your setup code here, to run once:
  Clamp.attach(servoPin); //links servo to the pin
}

void loop() {
  // put your main code here, to run repeatedly:
  if(angle == 0){
    angle = 45;
  } else{
    angle = 0;
  }
  Clamp.write(angle);
  delay(1000);
}
