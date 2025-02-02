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

// ==========================
// CALIBRATION VALUES
// ==========================
// (These should be set using your calibration sketch)
int redMin = 0;   // Red minimum value
int redMax = 0;   // Red maximum value
int greenMin = 0; // Green minimum value
int greenMax = 0; // Green maximum value
int blueMin = 0;  // Blue minimum value
int blueMax = 0;  // Blue maximum value

// ==========================
// COLOR CONSTANTS & PATTERN
// ==========================

// Use simple constants to represent colors
#define COLOR_NONE  0
#define COLOR_RED   0x01  // 1 in decimal (001 in binary)
#define COLOR_BLUE  0x02  // 2 in decimal (010 in binary)
#define COLOR_GREEN 0x04  // 4 in decimal (100 in binary)

// The required pattern: Red -> Green -> Blue -> Green -> Blue
const int patternLength = 5;
int pattern[patternLength] = { COLOR_RED, COLOR_GREEN, COLOR_BLUE, COLOR_GREEN, COLOR_BLUE };
int currentIndex = 0;  // which color in the pattern we are looking for

// ==========================
// VARIABLES FOR COLOR PULSE WIDTHS
// ==========================
int redPW = 0;
int greenPW = 0;
int bluePW = 0;

// (Optional mapping values – printed for debugging)
int redValue, greenValue, blueValue;

// ==========================
// VARIABLES FOR TILE DEBOUNCE/UNIQUE DETECTION
// ==========================
// We want to “count” a block only once.
// Also, if the same color appears twice in a row in the pattern,
// we want to be sure the second instance is a different block.
bool tileRegistered = false;        // true if the sensor is currently “on” a tile
unsigned long lastTileTime = 0;       // time when a tile was registered
const unsigned long tileCooldown = 2000; // minimum time (ms) to wait before counting the same color again
int lastRegisteredColor = COLOR_NONE; // stores the last registered color

// ==========================
// MOTOR DRIVE MODES
// ==========================
int forwardMode[4]  = { HIGH, LOW, HIGH, LOW };
int backwardMode[4] = { LOW, HIGH, LOW, HIGH };
int leftMode[4]     = { LOW, HIGH, HIGH, LOW };
int rightMode[4]    = { HIGH, LOW, LOW, HIGH };

// For timing driving maneuvers
int secPerRotation = 2800;  // time (ms) for a full rotation (adjust as needed)

// ==========================
// SETUP
// ==========================
void setup() {
  Serial.begin(9600);

  // Ultrasonic sensor setup
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

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
  
  // Attach servo (if needed)
  Clamp.attach(servoPin);

  // Set motor "speed" pins high (if using PWM later, adjust accordingly)
  digitalWrite(enA, HIGH);
  digitalWrite(enB, HIGH);

  // Color sensor setup
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  // Set pulse width scaling to 20%
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);
  // Sensor output as input
  pinMode(sensorOut, INPUT);
  
  // LED setup
  pinMode(LEDPin, OUTPUT);
}

// ==========================
// MAIN LOOP
// ==========================
void loop() {
  // If the complete pattern has been detected, stop the robot.
  if (currentIndex >= patternLength) {
    stop();
    Serial.println("Pattern complete!");
    while(true); // halt further execution
  }
  
  // Get the current color from the sensor
  int detectedColor = getColor();
  
  // --- TILE DEBOUNCE / UNIQUE DETECTION LOGIC ---
  // Only register a tile once. When no color is detected, reset the flag.
  if (detectedColor == COLOR_NONE) {
    tileRegistered = false;
  }
  else if (!tileRegistered) {
    // If the detected color matches the current expected pattern color...
    if (detectedColor == pattern[currentIndex]) {
      // For consecutive same-color entries in the pattern, require a “new” block.
      if (currentIndex > 0 && pattern[currentIndex] == pattern[currentIndex - 1]) {
        unsigned long currentTime = millis();
        if (detectedColor == lastRegisteredColor && (currentTime - lastTileTime) < tileCooldown) {
          Serial.println("Detected same tile again; waiting for a different tile.");
        }
        else {
          tileRegistered = true;
          lastTileTime = currentTime;
          lastRegisteredColor = detectedColor;
          blinkLED();
          currentIndex++;
          Serial.print("Tile accepted. Pattern progress: ");
          Serial.println(currentIndex);
        }
      }
      else {
        // For a non-repeated color in the pattern, register immediately.
        tileRegistered = true;
        lastTileTime = millis();
        lastRegisteredColor = detectedColor;
        blinkLED();
        currentIndex++;
        Serial.print("Tile accepted. Pattern progress: ");
        Serial.println(currentIndex);
      }
    }
  }
  
  // --- NAVIGATION & OBSTACLE AVOIDANCE ---
  // Adjust direction based on detected color.
  driveDirection(detectedColor);
  
  // Use the ultrasonic sensor to check for obstacles.
  int currentDistance = getDistance();
  if (currentDistance >= 10) {
    drive(forwardMode, 1000);
    currentDistance = getDistance();
    if (currentDistance >= 10) {
      drive(leftMode, secPerRotation * 0.50);
      drive(forwardMode, 1000);
      // Optionally adjust again based on color.
      driveDirection(detectedColor);
      drive(forwardMode, 1000);
    }
  }
}

// ==========================
// SUPPORT FUNCTIONS
// ==========================

// Blink the LED to give visual feedback for a correct tile detection.
void blinkLED() {
  digitalWrite(LEDPin, HIGH);
  delay(500);
  digitalWrite(LEDPin, LOW);
}

// Adjust the robot’s driving direction based on the currently detected color.
void driveDirection(int detectedColor) {
  if (detectedColor == COLOR_RED) {
    // For red, perform a sharper left turn.
    drive(leftMode, secPerRotation * 0.50);
  }
  else if (detectedColor == COLOR_BLUE) {
    // For blue, perform a slight left turn.
    drive(leftMode, secPerRotation * 0.25);
  }
  else if (detectedColor == COLOR_GREEN) {
    // For green, perform a slight right turn.
    drive(rightMode, secPerRotation * 0.25);
  }
}

// Read the sensor and decide what color is being seen.
int getColor() {
  // Get red reading
  redPW = getRedPW();
  redValue = map(redPW, redMin, redMax, 255, 0);
  delay(200);
  
  // Get green reading
  greenPW = getGreenPW();
  greenValue = map(greenPW, greenMin, greenMax, 255, 0);
  delay(200);
  
  // Get blue reading
  bluePW = getBluePW();
  blueValue = map(bluePW, blueMin, blueMax, 255, 0);
  delay(200);
  
  // Debug printout of pulse widths:
  Serial.print("Red PW = ");
  Serial.print(redPW);
  Serial.print(" - Green PW = ");
  Serial.print(greenPW);
  Serial.print(" - Blue PW = ");
  Serial.println(bluePW);
  
  // Determine the color based on the measured pulse widths.
  // (Adjust the thresholds below according to your calibration.)
  if (redPW < 100) {
    Serial.println("Red detected");
    return COLOR_RED;
  }
  else if (greenPW < 100 && bluePW < 100) {
    Serial.println("Green detected");
    return COLOR_GREEN;
  }
  else if (bluePW < 100) {
    Serial.println("Blue detected");
    return COLOR_BLUE;
  }
  else {
    Serial.println("No color detected");
    return COLOR_NONE;
  }
}

// Drive the motors using the specified mode and duration.
void drive(int driveMode[], int duration) {
  digitalWrite(in1, driveMode[0]);
  digitalWrite(in2, driveMode[1]);
  digitalWrite(in3, driveMode[2]);
  digitalWrite(in4, driveMode[3]);
  delay(duration);
}

// Stop all motors.
void stop() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}

// Use the ultrasonic sensor to measure the distance ahead.
int getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  long duration = pulseIn(echoPin, HIGH);
  int distance = duration * 0.034 / 2;
  
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  
  delay(500);
  return distance;
}

// --------------------------
// Color Sensor Pulse Width Functions
// --------------------------

// Read the red pulse width
int getRedPW() {
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  return pulseIn(sensorOut, LOW);
}

// Read the green pulse width
int getGreenPW() {
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  return pulseIn(sensorOut, LOW);
}

// Read the blue pulse width
int getBluePW() {
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  return pulseIn(sensorOut, LOW);
}
