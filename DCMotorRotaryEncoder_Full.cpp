#define GPSReceive Serial1

const int ENB_PIN = 7;
const int IN3_PIN = 6;
const int IN4_PIN = 5;
const int encoderA = 2;
const int encoderB = 3;

volatile int encoderPos = 0;
int targetValueAzimuth = 0;
int targetVal;

void setup() {
  Serial.begin(115200);
  GPSReceive.begin(19200);

  pinMode(ENB_PIN, OUTPUT);
  pinMode(IN3_PIN, OUTPUT);
  pinMode(IN4_PIN, OUTPUT);

  pinMode(encoderA, INPUT_PULLUP);
  pinMode(encoderB, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(encoderA), updateEncoder, CHANGE);

  digitalWrite(IN3_PIN, HIGH);
  digitalWrite(IN4_PIN, LOW);
  analogWrite(ENB_PIN, 100);
}

void CCWDC() { //move counter-clockwise
  digitalWrite(IN3_PIN, LOW);
  digitalWrite(IN4_PIN, HIGH);
}

void stopDC() { //stop
  digitalWrite(IN3_PIN, LOW);
  digitalWrite(IN4_PIN, LOW);
}

void CWDC() { // move clockwise
  digitalWrite(IN3_PIN, HIGH);
  digitalWrite(IN4_PIN, LOW);
}

void loop() {
  // Read encoder position
  int encoderReading = encoderPos;
   if (GPSReceive.available() > 1 ){
    targetVal = GPSReceive.read();
   }
   Serial.println(targetVal, BIN);
  targetValueAzimuth = targetVal * 3;
  // Calculate error between target position and current position
  if (encoderPos < targetValueAzimuth - 5){
    CWDC(); 
  } else if (encoderPos > targetValueAzimuth + 5){
    CCWDC();
  } else{
    stopDC();
  }
  
  // Print current position and target position to serial monitor
  Serial.print("Current position ");
  Serial.print(encoderReading);
  Serial.print(" and the Target position is ");
  Serial.println(targetValueAzimuth);
  delay(1000);
}

// Function to update encoder position
void updateEncoder() {
  int encoderA = digitalRead(encoderA);
  int encoderB = digitalRead(encoderB);
  
  if (encoderA == encoderB) {
    encoderPos++;
  } else {
    encoderPos--;
  }
}
