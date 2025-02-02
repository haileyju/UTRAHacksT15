#include <Servo.h>

/* 6,7, 8, 9, 10, 11 */
// Motor A connections
int enA = 11;
int in1 = 10;
int in2 = 9;
// Motor B connections
int enB = 8;
int in3 = 7;
int in4 = 6;

Servo Clamp; //creating a servo object called Clamp

int servoPin = 5; //outputting values the Servo Clamp
int angle = 0;

void setup() {
  // Set all the motor control pins to outputs
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  // Turn off motors - Initial state
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);


  Clamp.attach(servoPin); //links servo to the pin

  // For PWM maximum possible values are 0 to 1
  digitalWrite(enA, HIGH);
  digitalWrite(enB, HIGH);
}

void loop() {
  // Set motors to maximum speed


  int forward[4] = {HIGH, LOW, HIGH, LOW};
  int backward[4] = {LOW, HIGH, LOW, HIGH};
  int left[4] = {LOW, HIGH, HIGH, LOW};
  int right[4] = {HIGH, LOW, LOW, HIGH};

  int passedColors = 0;
  
  

  drive(forward, 2000);
  delay(2000);
  stop();
  delay(2000);
}

void startingSetup() {

}


void drive(int driveMode[], int d) {
  digitalWrite(in1, driveMode[0]);
  digitalWrite(in2, driveMode[1]);
  digitalWrite(in3, driveMode[2]);
  digitalWrite(in4, driveMode[3]);
  delay(d);
}

void stop() {

  // Turn off motors
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}
