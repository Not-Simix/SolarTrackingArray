#include <Wire.h>
#include <MPU6050.h>
#include <LiquidCrystal.h>

#define GPSReceive Serial1
#define AnemometerReceive Serial2

MPU6050 mpu;

// Set Motor Driver Connection Pins 
const int ENA_PIN = 7;
const int IN1_PIN = 6;
const int IN2_PIN = 5;
const int rs = 22, en = 23, d24 = 24, d25 = 25, d26 = 26, d27 = 27;
LiquidCrystal lcd(rs, en, d24, d25, d26, d27);
//Create the target value
int targetValueElevation = 0;
int AnemData = 1;
int targetVal;

void setup() {
  Serial.begin(115200);
  GPSReceive.begin(19200);
  AnemometerReceive.begin(115200);
  lcd.begin(16,2);
  pinMode(ENA_PIN, OUTPUT);
  pinMode(IN1_PIN, OUTPUT);
  pinMode(IN2_PIN, OUTPUT);
  lcd.print("TargetE  ActualE");

  digitalWrite(ENA_PIN, HIGH);
  Serial.println("Begin Actuator code");
    while(!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G)) {
    Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
    delay(500);
  }
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
  if (AnemometerReceive.available() >= 2){
    AnemData = AnemometerReceive.read(); //reads whether feathering needs to take place
  }
  if (GPSReceive.available() >= 2  ){
    targetVal = GPSReceive.read();
    //targetVal = 0;
  }

  Serial.println(AnemData);
  Serial.print("Target Val: ");
  Serial.println(targetVal);
  // Uncomment to debug
  // Serial.println("Anem Data read complete");
  if (AnemData == 1) { // normal functioning
    targetValueElevation = targetVal;
  } else { // Feather mode when AnemData = 0
    targetValueElevation = 0;
  }
  if (targetValueElevation > 90) {
    targetValueElevation = 0;
  } else { 
    targetValueElevation = targetValueElevation;
  }
  // Uncomment to debug
  // Serial.println("Anem Data argument complete");
  // Read normalized values 
Vector normAccel = mpu.readNormalizeAccel();
// Calculate Solar Panel Elevation
  int elevation = abs(-(atan2(normAccel.XAxis, sqrt(normAccel.YAxis*normAccel.YAxis + normAccel.ZAxis*normAccel.ZAxis))*180.0)/M_PI);
 
 // Uncomment to debug
  // Serial.println("elevation of solar panel calculated");

  // Output
Serial.print("Elevation = ");
Serial.print(elevation);
Serial.print(", Target value elevation = ");
Serial.print(targetValueElevation);
Serial.print(", AnemData = ");
Serial.println(AnemData);
lcd.setCursor(0,1);
lcd.print(targetValueElevation);
lcd.setCursor(9,1);
lcd.print(elevation);

  //The following determines if the Solar panel is within +- 5 degrees of the suns elevation
  if (elevation < targetValueElevation - 5) {
    extendActuator();
  }
  else if (elevation > targetValueElevation + 5) {
    retractActuator();
  }
  else{
    stopActuator();
  }
}
