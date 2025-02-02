const int trigPin = 12;
const int echoPin = 13;

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  
  // Set trigPin as output and echoPin as input
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
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
}
