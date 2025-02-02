#include <Servo.h>

// ==========================
// PIN & SENSOR DEFINITIONS
// ==========================

// Color sensor pins
#define S0 0
#define S1 1
#define S2 2
#define S3 3
#define sensorOut 4

// Ultrasonic sensor pins
const int trigPin = 12;
const int echoPin = 13;

// Motor A connections (left motor)
int enA = 11;
int in1 = 10;
int in2 = 9;
// Motor B connections (right motor)
int enB = 8;
int in3 = 7;
int in4 = 6;

// LED for feedback (blink on correct detection)
int LEDPin = A0; // Adjust if needed

// Servo for the claw (if used)
Servo Clamp;
int servoPin = 5;
int angle = 0;
int servoOpenClose = 0; // zero is closed, 1 is open

// ==========================
// CALIBRATION VALUES
// ==========================
// (These should be set using your calibration sketch)
// *Get these from Calibration Sketch
int redMin = 213; // Red minimum value
int redMax = 1365; // Red maximum value
int greenMin = 207; // Green minimum value
int greenMax = 1243; // Green maximum value
int blueMin = 180; // Blue minimum value
int blueMax = 1204; // Blue maximum value

// Variables for Color Pulse Width Measurements
int redPW = 0;
int greenPW = 0;
int bluePW = 0;

// Variables for final Color values
int redValue;
int greenValue;
int blueValue;


// ==========================
// COLOR CONSTANTS & PATTERN
// ==========================

// Use simple constants to represent colors
#define COLOR_NONE  0
#define COLOR_RED   0x01  // 1 in decimal (001 in binary)
#define COLOR_BLUE  0x02  // 2 in decimal (010 in binary)
#define COLOR_GREEN 0x04  // 4 in decimal (100 in binary)
#define COLOR_WHITE 0x07  // 4 in decimal (100 in binary)



// ==========================
// MOTOR DRIVE MODES
// ==========================
int forwardMode[4] = { HIGH, LOW, HIGH, LOW };
int backwardMode[4] = { LOW, HIGH, LOW, HIGH };
int leftMode[4] = { LOW, HIGH, HIGH, LOW };
int rightMode[4] = { HIGH, LOW, LOW, HIGH };

// For timing driving maneuvers
int secPerRotation = 2800;  // time (ms) for a full rotation (adjust as needed)

// ==========================
// SETUP
// ==========================
void setup() {
	// Set S0 - S3 as outputs
	pinMode(S0, OUTPUT);
	pinMode(S1, OUTPUT);
	pinMode(S2, OUTPUT);
	pinMode(S3, OUTPUT);

	// Set Pulse Width scaling to 20%
	digitalWrite(S0, HIGH);
	digitalWrite(S1, LOW);

	// Set Sensor output as input
	pinMode(sensorOut, INPUT);


	// Motor pins setup
	pinMode(enA, OUTPUT);
	pinMode(enB, OUTPUT);
	pinMode(in1, OUTPUT);
	pinMode(in2, OUTPUT);
	pinMode(in3, OUTPUT);
	pinMode(in4, OUTPUT);

	// Initialize motors to off
	digitalWrite(in1, LOW);
	digitalWrite(in2, LOW);
	digitalWrite(in3, LOW);
	digitalWrite(in4, LOW);

	// Set motor "speed" pins high (if using PWM later, adjust accordingly)
	digitalWrite(enA, HIGH);
	digitalWrite(enB, HIGH);

	// Setup Serial Monitor
	Serial.begin(9600);
	Clamp.attach(servoPin); //links servo to the pin

	startingSetup();
}

// ==========================
// GAME LOGIC
// ==========================

int numColorPassed = 0; // stop when 4;
int currentColor = 0;
int prevColor = 0;


void loop() {
	int detectedColor = getColor();

	// Serial.println(numColorPassed);
	if (numColorPassed > 3) {
		// Serial.println("GAME ENDED");
		stop();
		angle = 45;
		Clamp.write(angle);
		angle = 0;
		Clamp.write(angle);
		angle = 45;
		Clamp.write(angle);
		while (true);
	}

	// Serial.println(detectedColor);
	// drive(forwardMode, 400);

	zig();
	if (detectedColor != currentColor) {
		prevColor = currentColor;
		currentColor = detectedColor;
		numColorPassed++;
	}
	stop();
	delay(1000);

	detectedColor = getColor();
	zag();
	if (detectedColor != currentColor) {
		prevColor = currentColor;
		currentColor = detectedColor;
		numColorPassed++;
	}

	stop();
	delay(1000);

}

int getColor() {
	// Read Red Pulse Width
	redPW = getRedPW();
	// Map to value from 0-255
	redValue = map(redPW, redMin, redMax, 255, 0);
	// Delay to stabilize sensor
	delay(200);

	// Read Green Pulse Width
	greenPW = getGreenPW();
	// Map to value from 0-255
	greenValue = map(greenPW, greenMin, greenMax, 255, 0);
	// Delay to stabilize sensor
	delay(200);

	// Read Blue Pulse Width
	bluePW = getBluePW();
	// Map to value from 0-255
	blueValue = map(bluePW, blueMin, blueMax, 255, 0);
	// Delay to stabilize sensor
	delay(200);

	// // Print output to Serial Monitor
	// Serial.print("Red PW = ");
	// Serial.print(redPW);
	// Serial.print(" - Green PW = ");
	// Serial.print(greenPW);
	// Serial.print(" - Blue PW = ");
	// Serial.println(bluePW);

	// // Print output to Serial Monitor
	// Serial.print("Red PW = ");
	// Serial.print(redValue);
	// Serial.print(" - Green PW = ");
	// Serial.print(greenValue);
	// Serial.print(" - Blue PW = ");
	// Serial.println(blueValue);

	if (redValue < 100 && blueValue < 100 && greenValue < 100) {
		// Serial.println("Black is detected");
		return COLOR_NONE;
	}
	else if (redValue > greenValue && redValue > blueValue) {
		// Serial.println("Red is Detected");
		return COLOR_RED;
	}
	else if (greenValue > blueValue && greenValue > redValue) {
		// Serial.println("Green is Detected");
		return COLOR_GREEN;
	}
	else if (blueValue > redValue && blueValue > greenValue) {
		// Serial.println("Blue is Detected");
		return COLOR_BLUE;

	}
	else {
		//  Serial.println("Nothing is Detected"); 
		return COLOR_WHITE;
	}
}

void startingSetup() {
	int detectedColor = getColor();

  drive(backwardMode, 100);

  int turn[] = {HIGH, LOW, LOW, LOW};
	while (detectedColor != COLOR_BLUE && detectedColor != COLOR_RED && detectedColor != COLOR_GREEN) {
		drive(turn, secPerRotation * 0.1666666667);
		detectedColor = getColor();
		delay(200);
	}
}

// Drive the motors using the specified mode and duration.
void drive(int driveMode[], float duration) {
	digitalWrite(in1, driveMode[0]);
	digitalWrite(in2, driveMode[1]);
	digitalWrite(in3, driveMode[2]);
	digitalWrite(in4, driveMode[3]);
	delay(duration);
}

void stop() {

	// Turn off motors
	digitalWrite(in1, LOW);
	digitalWrite(in2, LOW);
	digitalWrite(in3, LOW);
	digitalWrite(in4, LOW);
}


void zig() {
	drive(leftMode, secPerRotation * 0.125);
	drive(forwardMode, 500);
	stop();
}

void zag() {
	drive(rightMode, secPerRotation * 0.125);
	drive(forwardMode, 500);
	stop();
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
