#include <Wire.h>
#include <MPU6050.h>
#include <LiquidCrystal_I2C.h>
#define GPSReceive Serial1
#define AnemometerReceive Serial2

MPU6050 mpu; //Define the inclinometer

// Set Motor Driver Connection Pins 
const int ENA_PIN = 7;
const int IN1_PIN = 6;
const int IN2_PIN = 5;
//Create the target value
int targetValueElevation = 0;
int AnemData = 1;
int targetVal;

void setup() {
  //Open all serial communication ports
  Serial.begin(115200);
  GPSReceive.begin(19200);
  AnemometerReceive.begin(115200);

  //Set Motor Driver pins to outputs
  pinMode(ENA_PIN, OUTPUT);
  pinMode(IN1_PIN, OUTPUT);
  pinMode(IN2_PIN, OUTPUT);

  //Set the Motor driver speed to HIGH
  digitalWrite(ENA_PIN, HIGH);
  
  //Begin functioning when Inclinometer is found
    while(!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G)) {
    Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
    delay(500);
  }
}

void retractActuator() { //Pull actuator inwards
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, HIGH);
}

void stopActuator() { //Stop actuator
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, LOW);
}

void extendActuator() { //Push actuator outwards
  digitalWrite(IN1_PIN, HIGH);
  digitalWrite(IN2_PIN, LOW);
}

void loop() {
  if (AnemometerReceive.available() >= 1){ //Check if data is being received from the wind speed calculations
    AnemData = AnemometerReceive.read(); //reads whether feathering needs to take place
  }
  if (GPSReceive.available() >= 1){ //Check if data is being received from the Orientation calculations
    targetVal = GPSReceive.read();
  }

  Serial.println(AnemData); //Print to the PC the feathering flag
  Serial.print("Target Val: ");
  Serial.println(targetVal); //Print the elevation target

  if (AnemData == 1) { // normal functioning
    targetValueElevation = targetVal;
  } else { // Feather mode when AnemData = 0
    targetValueElevation = 0;
  }
  if (targetValueElevation > 90) { //Stops the system from going over 90 degrees
    targetValueElevation = 0;

  } else { // normal functioning
    targetValueElevation = targetValueElevation;
  }

Vector normAccel = mpu.readNormalizeAccel();
// Calculate Solar Panel Elevation
int elevation = abs(-(atan2(normAccel.XAxis, sqrt(normAccel.YAxis*normAccel.YAxis + normAccel.ZAxis*normAccel.ZAxis))*180.0)/M_PI);

// Output
Serial.print("Elevation = ");
Serial.print(elevation);
Serial.print(", Target value elevation = ");
Serial.print(targetValueElevation);
Serial.print(", AnemData = ");
Serial.println(AnemData);
//The following determines if the Solar panel is within +- 5 degrees of the suns elevation
if (elevation < targetValueElevation - 3) { //Extend the actuator to reach the target
  extendActuator();
  Serial.println("extending");
} 
  else if (elevation > targetValueElevation + 3) {//Retract the actuator to reach the target
    retractActuator();
    Serial.println("retracting");
  }
  else{ //Stop the actuator
    stopActuator();
    Serial.println("stop");
  }
}
