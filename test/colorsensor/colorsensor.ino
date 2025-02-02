#define PIN_S0 0
#define PIN_S1 1
#define PIN_S2 2
#define PIN_S3 3
#define PIN_OUT 4
/*
const int s0 = 0;
const int s1 = 1;
const int s2 = 2;
const int s3 = 3;
const int out = 4;
*/
// Color values
 int red = 0;
 int green = 0;
 int blue = 0;

// Calibration values
 int cal_min_r, cal_max_r;
 int cal_min_g, cal_max_g;
 int cal_min_b, cal_max_b;

// Function to read color sensor values
 void readColor() {

  digitalWrite(PIN_S2, LOW);
  digitalWrite(PIN_S3, LOW);
  int red = pulseIn(PIN_OUT, HIGH);

  digitalWrite(PIN_S2, HIGH);
  digitalWrite(PIN_S3, LOW);
  //int blue = pulseIn(PIN_OUT, digitalRead(PIN_OUT) == HIGH ? LOW : HIGH);
  int blue = pulseIn(PIN_OUT, HIGH);

  digitalWrite(PIN_S2, LOW);
  digitalWrite(PIN_S3, HIGH);  
  int green = pulseIn(PIN_OUT, HIGH);
//   digitalWrite(PIN_S2, LOW);
//   digitalWrite(PIN_S3, LOW);
//   int red = pulseIn(PIN_OUT, HIGH);

//   digitalWrite(PIN_S2, HIGH);
//   digitalWrite(PIN_S3, LOW);
//   //int blue = pulseIn(PIN_OUT, digitalRead(PIN_OUT) == HIGH ? LOW : HIGH);
//   int blue = pulseIn(PIN_OUT, HIGH);

//   digitalWrite(PIN_S2, LOW);
//   digitalWrite(PIN_S3, HIGH);  
//   int green = pulseIn(PIN_OUT, HIGH);
// ]
/*
  //int largestColour = 0;
  for(int i = 1; i < 4; i++){
    if((i == 1) && (red > largestColour)){
      largestColour = red;
    }else if((i == 2) && (blue > largestColour)){
      largestColour = blue;
    }else if((i == 3) && (green > largestColour))
      largestColour = green
  }
  
  if(largestColour = red){
    return red;
  }else if(largestColour = blue){
    return blue;
  }else if(largestColour = green){
    return green;
  }
  */
 }
 //Calibration function
 void calibrate() {
   Serial.println("Place BLACK object in front of sensor, then enter 'c'");
   while (Serial.read() != 'c');  // Wait for user input
   readColor();
   cal_min_r = red;
   cal_min_g = green;
   cal_min_b = blue;

   Serial.println("Place WHITE object in front of sensor, then enter 'c'");
   while (Serial.read() != 'c');  // Wait for user input
   readColor();
   cal_max_r = red;
   cal_max_g = green;
   cal_max_b = blue;

   Serial.println("Calibration completed.");
}

// Setup function
void setup() {
  Serial.begin(9600);
  pinMode(PIN_S0, OUTPUT);
  pinMode(PIN_S1, OUTPUT);
  pinMode(PIN_S2, OUTPUT);
  pinMode(PIN_S3, OUTPUT);
  pinMode(PIN_OUT, INPUT);

  digitalWrite(PIN_S0, HIGH);
  digitalWrite(PIN_S1, HIGH);
}

// Main loop
void loop() {
   if (Serial.read() == 'c') {
     calibrate();
  }

   //readColor();

  // int mappedRed = map(red, cal_min_r, cal_max_r, 255, 0);
  // int mappedGreen = map(green, cal_min_g, cal_max_g, 255, 0);
  // int mappedBlue = map(blue, cal_min_b, cal_max_b, 255, 0);

  // // Constrain values between 0-255
  // mappedRed = constrain(mappedRed, 0, 255);
  // mappedGreen = constrain(mappedGreen, 0, 255);
  // mappedBlue = constrain(mappedBlue, 0, 255);

 // void readColor() {
  // digitalWrite(PIN_S2, LOW);
  // digitalWrite(PIN_S3, LOW);
  // int red = pulseIn(PIN_OUT, HIGH);

  // digitalWrite(PIN_S2, HIGH);
  // digitalWrite(PIN_S3, LOW);
  // //int blue = pulseIn(PIN_OUT, digitalRead(PIN_OUT) == HIGH ? LOW : HIGH);
  // int blue = pulseIn(PIN_OUT, HIGH);

  // digitalWrite(PIN_S2, LOW);
  // digitalWrite(PIN_S3, HIGH);  
  // int green = pulseIn(PIN_OUT, HIGH);
//]

  // Print detected color
  if (red > green && red > blue) {
    Serial.println("r"); // Red detected
  } 
  else if (green > red && green > blue) {
    Serial.println("g"); // Green detected
  } 
  else if (blue > red && blue > green) {
    Serial.println("b"); // Blue detected
  } else{
    Serial.println("no");
  }
  delay(200);
}
