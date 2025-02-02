#include <Servo.h>

// Define color sensor pins
#define S0 0
#define S1 1
#define S2 2
#define S3 3
#define sensorOut 4

const int trigPin = 12;
const int echoPin = 13;


// Calibration Values
// *Get these from Calibration Sketch
int redMin = 0; // Red minimum value
int redMax = 0; // Red maximum value
int greenMin = 0; // Green minimum value
int greenMax = 0; // Green maximum value
int blueMin = 0; // Blue minimum value
int blueMax = 0; // Blue maximum value

// Variables for Color Pulse Width Measurements
int redPW = 0;
int greenPW = 0;
int bluePW = 0;

// Variables for final Color values
int redValue;
int greenValue;
int blueValue;


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

int secPerRotation = 2800;

/* 
  red = 001
  blue = 010
  green = 100
  none = 000
*/
int prevColor = 000;
int currentColor = 000;
int nextColor = 000;

int LEDPin = A0; // ? TO CHANGE


int currentPattern = 000;
int currentIndex = 0;
int pattern[5] = {001, 100, 010, 100, 010}; // Red -> Greens -> Blue -> Green -> Blue

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  
  // Set trigPin as output and echoPin as input
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
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


  // * USED FOR COLOR SETTING
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);

  // Set Pulse Width scaling to 20%
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);

  // Set Sensor output as input
  pinMode(sensorOut, INPUT);

}

bool started = true;

  int forward[4] = { HIGH, LOW, HIGH, LOW };
  int backward[4] = { LOW, HIGH, LOW, HIGH };
  int left[4] = { LOW, HIGH, HIGH, LOW };
  int right[4] = { HIGH, LOW, LOW, HIGH };

/* 
  left = 001
  uturn = 010
  right = 100
  none = 000
*/


void loop() {
  // Set motors to maximum speed
  if (currentIndex == 5) {
    stop();
    return;
  }

  int detectedColor = getColor();

  if (detectedColor == pattern[currentIndex]) {
    digitalWrite(LEDPin, HIGH);
    delay(500);
    digitalWrite(LEDPin, LOW);
    currentIndex++;
  }

  driveDirection(detectedColor);

  int currentDistance = getDistance();

  if (currentDistance >= 10) {
    drive(forward, 1000);
    currentDistance = getDistance();
     if (currentDistance >= 10) {
      drive(left, secPerRotation * 0.50);
      drive(forward, 1000);
      driveDirection(detectedColor);
      drive(forward, 1000);
     }
  }
}

void driveDirection(int detectedColor) {
  if (detectedColor == 001) { // red
    drive(left, secPerRotation * 0.50);
  }

  if (detectedColor == 010) { // blue
    drive(left, secPerRotation * 0.25);
  }

  if (detectedColor == 100) { // green
    drive(right, secPerRotation * 0.25);
  }
}
int getColor() {
  	// Read Red Pulse Width
	redPW = getRedPW();
  // Map to value from 0-255
  redValue = map(redPW, redMin,redMax,255,0);
	// Delay to stabilize sensor
	delay(200);

	// Read Green Pulse Width
	greenPW = getGreenPW();
  // Map to value from 0-255
  greenValue = map(greenPW, greenMin, greenMax,255,0);
	// Delay to stabilize sensor
	delay(200);

	// Read Blue Pulse Width
	bluePW = getBluePW();
  // Map to value from 0-255
  blueValue = map(bluePW, blueMin, blueMax,255,0);
	// Delay to stabilize sensor
	delay(200);

	// Print output to Serial Monitor
	Serial.print("Red PW = ");
	Serial.print(redPW);
	Serial.print(" - Green PW = ");
	Serial.print(greenPW);
	Serial.print(" - Blue PW = ");
	Serial.println(bluePW);

  if (redPW < 100) {
    Serial.println("Red is Detected");
    return 001;
  }
  else if (greenPW < 100 && bluePW < 100) {
    Serial.println("Green is Detected");
    return 100;
  }
  else if (bluePW < 100){
    Serial.println("Blue is Detected");
    return 010;
  } 
  else {
    Serial.println("Nothing is Detected");
    return 000;
  }
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

int getDistance () {
  // Clear the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Send a 10-microsecond pulse to trigger
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read the echo time (in microseconds)
  long duration = pulseIn(echoPin, HIGH);

  // Calculate distance in cm (speed of sound = 34300 cm/s)
  int distance = duration * 0.034 / 2;

  // Print distance to the Serial Monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Wait a bit before the next measurement
  delay(500);
  return distance;
}


// Function to read Red Pulse Widths
int getRedPW() {
  // Set sensor to read Red only
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  // Define integer to represent Pulse Width
  int PW;
  // Read the output Pulse Width
  PW = pulseIn(sensorOut, LOW);
  // Return the value
  return PW;
}

// Function to read Green Pulse Widths
int getGreenPW() {
  // Set sensor to read Green only
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  // Define integer to represent Pulse Width
  int PW;
  // Read the output Pulse Width
  PW = pulseIn(sensorOut, LOW);
  // Return the value
  return PW;
}

// Function to read Blue Pulse Widths
int getBluePW() {
  // Set sensor to read Blue only
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  // Define integer to represent Pulse Width
  int PW;
  // Read the output Pulse Width
  PW = pulseIn(sensorOut, LOW);
  // Return the value
  return PW;
}
