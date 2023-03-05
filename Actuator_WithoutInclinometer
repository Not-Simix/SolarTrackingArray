#define GPSReceive Serial1
#define AnemometerReceive Serial2

// Set Motor Driver Connection Pins 
const int ENA_PIN = 7;
const int IN1_PIN = 6;
const int IN2_PIN = 5;

//Create the target value
int targetValueElevation = 0;
int AnemData = 1;

void setup() {
  Serial.begin(115200);
  GPSReceive.begin(19200);
  AnemometerReceive.begin(115200);
  pinMode(ENA_PIN, OUTPUT);
  pinMode(IN1_PIN, OUTPUT);
  pinMode(IN2_PIN, OUTPUT);

  digitalWrite(ENA_PIN, HIGH);
  Serial.println("Begin Actuator code");
}

void retractActuator() {
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, HIGH);
}

void stopActuator() {
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, LOW);
}

void extendActuator() {
  digitalWrite(IN1_PIN, HIGH);
  digitalWrite(IN2_PIN, LOW);
}

void loop() {
  //while(!AnemometerReceive.available()){
  AnemData = AnemometerReceive.read(); //reads whether feathering needs to take place
  //}// Uncomment to debug
  // Serial.println("Anem Data read complete");
  if (AnemData == 1) { // normal functioning
    while (GPSReceive.available()){targetValueElevation = GPSReceive.read();
      }
    }else { // Feather mode when AnemData = 0
    targetValueElevation = 0;
  }
  // Uncomment to debug
  // Serial.println("Anem Data argument complete");
 int elevation = 30;
 // Uncomment to debug
  // Serial.println("elevation of solar panel calculated");

  // Output
Serial.print("Elevation = ");
Serial.print(elevation);
Serial.print(", Target value elevation = ");
Serial.print(targetValueElevation);
Serial.print(", AnemData = ");
Serial.println(AnemData);
Serial.println(GPSReceive.read());

  //The following determines if the Solar panel is within +- 3 degrees of the suns elevation
  if (elevation < targetValueElevation - 3) {
    extendActuator();
  }
  else if (elevation > targetValueElevation + 3) {
    retractActuator();
  }
  else{
    stopActuator();
  }
  delay(1000);
}
