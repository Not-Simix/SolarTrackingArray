#include <Wire.h>

// Set Motor Driver Connection Pins 
const int ENA_PIN = 7;
const int IN1_PIN = 6;
const int IN2_PIN = 5;

//Create the target value
int targetValueElevation = 0;

void setup() {
  Wire.begin(11)
  Serial.begin(115200);
  pinMode(ENA_PIN, OUTPUT);
  pinMode(IN1_PIN, OUTPUT);
  pinMode(IN2_PIN, OUTPUT);

  digitalWrite(ENA_PIN, HIGH);
  Serial.println("Begin Actuator code")
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
  while(Wire.available()) {
    targetValueElevation = Wire.read();
  }
  while(Wire.available()) {
    elevation = Wire.read();
  }

  // Output
Serial.print("Elevation = ");
Serial.print(elevation);
Serial.print(", Target value elevation = ");
Serial.print(targetValueElevation);

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
  delay(10);
}
