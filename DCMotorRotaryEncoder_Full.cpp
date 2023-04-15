#include <LiquidCrystal_I2C.h>

#define GPSReceive Serial1

const int EN = 7; //Enable Motor Driver PWM Control
const int IN1 = 6; //Set IN1 pinout
const int IN2 = 5; //Set IN2 pinout

const int encoderA = 2; //Set encoder channel A pinout
const int encoderB = 3; //Set encoder channel B pinout

volatile int counter = 0; //Create a counter
volatile int temp = 0; //Create a temp data storage
int targetValueAzimuth = 0; //Set the targetValueAzimuth for Azimuth logic
int targetVal; //Set the received value from Orientation Logic
int gearRatio = 100; //Specify the gear ratio of the system
bool retHome = false; //Set the flag to return home

LiquidCrystal_I2C lcd(0x3F,16,2); //Create an LCD Screen

void setup() {
  Serial.begin(115200); //Open up serial communication to a PC
  GPSReceive.begin(19200); //Open up serial communication with the Orientation calculations

  lcd.init(); //Initialize LCD
  lcd.clear(); //Clear LCD
  lcd.backlight(); //Start the LCD backlight
  lcd.print("TargetA  ActualA"); //Set static text on the LCD

  pinMode(EN, OUTPUT); //Set motor driver pins to outputs
  pinMode(IN1, OUTPUT); //Set motor driver pins to outputs
  pinMode(IN2, OUTPUT); //Set motor driver pins to outputs

  pinMode(encoderA, INPUT_PULLUP); //Set encoder channel pins to input pullups
  pinMode(encoderB, INPUT_PULLUP); //Set encoder channel pins to input pullups
  attachInterrupt(0, ai0, RISING); //Create an interrupt for when voltage on channel A rises
  attachInterrupt(1, ai1, RISING); //Create an interrupt for when voltage on channel B rises

}

void CCWDC() { //move the system counter-clockwise
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(EN, 255); //Set speed to fastest PWM signal
}

void stopDC() { //stop
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(EN,0); //Set speed to stop
}

void CWDC() { // move clockwise
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(EN, 255); //Set speed to fastest PWM signal
}

void loop() {

  if(counter/gearRatio == 720 && !retHome){ //Coded limit of 720 degrees of rotation in the system
    retHome = true; //Flags to return home
    targetVal = 0; //Sets home position
  } 
  if(counter/gearRatio == 0 && retHome){ //Flags to stop returning home
    retHome = false; //Flags to return to normal functioning
    targetVal = 0; //Buffers the home position
  }
  else{
      Serial.println(counter); //Print the counter
  if (GPSReceive.available() > 1 ){ //Checks that data is coming from the Orientation Logic
    targetVal = GPSReceive.read(); //Sets the received target Val
    Serial.println("Targ Received"); //Notifies PC the target was received
  }
  }
  targetValueAzimuth = targetVal * gearRatio; //Creates a logical position to move the system to
  // Calculate error between target position and current position
  if (counter < targetValueAzimuth - (gearRatio * 3)){ //Moves the system towards the target clockwise
    CWDC(); 
  } else if (counter > targetValueAzimuth + (gearRatio * 3)){ //Moves the system towards the target counter-clockwise
    CCWDC();
  } else{ //Stops the system
    stopDC();
  }
  // Print current position and target position to serial monitor
  Serial.print("Current position ") ;
  Serial.print(counter);
  Serial.print(" and the Target position is ");
  Serial.println(targetValueAzimuth);
  lcd.setCursor(0,1); //Prints the target value
  lcd.print(targetValueAzimuth/gearRatio);
  lcd.setCursor(9,1); //Prints the current value
  lcd.print(counter/gearRatio);
  delay(100);
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
