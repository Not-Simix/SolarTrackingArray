#include <LiquidCrystal.h>

#define GPSReceive Serial1

const int EN = 7;
const int IN1 = 6;
const int IN2 = 5;

const int encoderA = 2;
const int encoderB = 3;

volatile int counter = 0;
volatile int temp = 0;
int targetValueAzimuth = 0;
int targetVal;

const int rs = 22, en = 23, d24 = 24, d25 = 25, d26 = 26, d27 = 27;
LiquidCrystal lcd(rs, en, d24, d25, d26, d27);

void setup() {
  Serial.begin(115200);
  GPSReceive.begin(19200);
  lcd.begin(16,2);
  pinMode(EN, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  pinMode(encoderA, INPUT_PULLUP);
  pinMode(encoderB, INPUT_PULLUP);
  attachInterrupt(0, ai0, RISING);
  attachInterrupt(1, ai1, RISING);
  lcd.print("TargetA  ActualA");
}

void CCWDC() { //move counter-clockwise
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(EN, 255);
}

void stopDC() { //stop
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(EN,0);
}

void CWDC() { // move clockwise
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(EN, 255);
}

void loop() {
  // Read encoder position
    // if( counter != temp ){
    // Serial.println (counter);
    // temp = counter;
    // }
  Serial.println(counter);
  if (GPSReceive.available() > 1 ){
    targetVal = GPSReceive.read();
    //targetVal  = -150;
  }
  targetValueAzimuth = targetVal * 30;
  // Calculate error between target position and current position
  if (counter < targetValueAzimuth - 150){
    CWDC(); 
  } else if (counter > targetValueAzimuth + 150){
    CCWDC();
  } else{
    stopDC();
  }

  // Print current position and target position to serial monitor
  Serial.print("Current position ") ;
  Serial.print(counter);
  Serial.print(" and the Target position is ");
  Serial.println(targetValueAzimuth);
  lcd.setCursor(0,1);
  lcd.print(targetValueAzimuth/30);
  lcd.setCursor(9,1);
  lcd.print(counter/30);
  delay(1000);
}

  void ai0() {
  // ai0 is activated if DigitalPin nr 2 is going from LOW to HIGH
  // Check pin 3 to determine the direction
  if(digitalRead(3)==LOW) {
  counter++;
  }else{
  counter--;
  }
  }
   
  void ai1() {
  // ai0 is activated if DigitalPin nr 3 is going from LOW to HIGH
  // Check with pin 2 to determine the direction
  if(digitalRead(2)==LOW) {
  counter--;
  }else{
  counter++;
  }
  }
