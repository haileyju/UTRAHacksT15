/* 6,7, 8, 9, 10, 11 */
// Motor A connections
int enA = 11;
int in1 = 10;
int in2 = 9;
// Motor B connections
int enB = 8;
int in3 = 7;
int in4 = 6;

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

}

void loop() {
	// Set motors to maximum speed
	// For PWM maximum possible values are 0 to 255
	digitalWrite(enA, HIGH);
	digitalWrite(enB, HIGH);
  directionControl();
  delay(2000);
  stopControl();
  delay(2000);
}


void directionControl() {
	// Turn on motor A & B
	digitalWrite(in1, HIGH);
	digitalWrite(in2, LOW);
	digitalWrite(in3, HIGH);
	digitalWrite(in4, LOW);
	delay(2000);
}


void goBack() {
  	// Now change motor directions
	digitalWrite(in1, LOW);
	digitalWrite(in2, HIGH);
	digitalWrite(in3, LOW);
	digitalWrite(in4, HIGH);
	delay(2000);
	
}

void turnRight() {
  // Now change motor directions
	digitalWrite(in1, HIGH);
	digitalWrite(in2, LOW);
	digitalWrite(in3, LOW);
	digitalWrite(in4, HIGH);
	delay(2000);
	
}

void turnLeft() {
  // Now change motor directions
	digitalWrite(in1, LOW);
	digitalWrite(in2, HIGH);
	digitalWrite(in3, HIGH);
	digitalWrite(in4, LOW);
	delay(2000);
	
}
void stopControl() {

	// Turn off motors
	digitalWrite(in1, LOW);
	digitalWrite(in2, LOW);
	digitalWrite(in3, LOW);
	digitalWrite(in4, LOW);

}