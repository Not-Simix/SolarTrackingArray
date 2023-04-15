#define TransmitActuator Serial1

int AnemData = 1; //Set Flag for Actuator Logic

bool isCounting = false; //Create a counting flag
bool isCountingSecondary = false; //Create a second counting flag

unsigned long startTime = 0; //Set a start time

void setup() {
  Serial.begin(115200); //Open a serial port for PC communication at 115200 Baud rate
  TransmitActuator.begin(115200); //Open a serial port for Windspeed -> Actuator logic communication at 115200 Baud rate
}

void loop() {
  unsigned long currTime = millis(); //Read the current time
  float sensorValue = analogRead(A0); //Read the anemometer sensor value
  Serial.print("Analog Value =");
  Serial.println(sensorValue);
 
  float voltage = (sensorValue / 1024) * 5; //Arduino ADC resolution 0-1023

  float wind_speed = mapfloat(voltage, 0.4, 2, 0, 32.4); //Translate the voltage into wind speed
  float speed_mph = ((wind_speed *3600)/1609.344); //Calculate mph based off wind speed
  float speed_knot = abs((wind_speed * 1.943844)); //Calculate knots based off wind speed
  Serial.print("Wind Speed ="); //Print the wind speed to the PC Serial port
  Serial.print(wind_speed); //Print the wind speed to the PC Serial port
  Serial.println("m/s");//Print the wind speed to the PC Serial port
  // Serial.print(speed_mph); //Print the wind speed to the PC Serial port
  // Serial.println("mph");//Print the wind speed to the PC Serial port
  Serial.print(speed_knot); //Print the wind speed to the PC Serial port
  Serial.println("knots"); //Print the wind speed to the PC Serial port
  Serial.println(" ");

  if (!isCounting && speed_knot > 35) { //Speed hits 35 knots for the first time
      isCounting = true; // flags begin feathering
      startTime = currTime; //Sets start of counting to current time
  }

   if (currTime > startTime + 15000 && isCounting) { //Time threshold has been reached
      AnemData = 0; // Flags actuator logic to start feathering
      isCounting = false; // stops flag to begin feathering
   }

   if (speed_knot < 35 && currTime < startTime + 15000 && isCounting) { //It's been less than 15 seconds and our speed has reduced
      isCounting = false; // stops flag to begin feathering
   }

  if (speed_knot < 30 && AnemData == 0 && !isCountingSecondary) { //Speed dips below 30 for the first time after being at 35
      isCountingSecondary = true; // flags end feathering
      startTime = currTime; //Sets start of counting to current time
  }

  if (currTime > startTime + 600000 && isCountingSecondary) { //Time threshold has been reached
      AnemData = 1; // Flags actuator logic to resume normal functioning
      isCountingSecondary = false; // stops flag to end feathering
  }

  if (speed_knot > 30 && currTime < startTime + 600000 && isCountingSecondary) { //It's been less than 10 minutes and our speed has increased
      isCountingSecondary = false; // stops flag to end feathering
  }

  TransmitActuator.write(AnemData); //Transmit the flag to the Actuator logic
  Serial.print(speed_knot);
  Serial.print("   ");
  Serial.println(AnemData);
  delay(100);
}

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max) { //Open source anemometer code which allows the voltage to be processed into wind speed
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
