#include <Wire.h>

int AnemData = 1;

bool isCounting = false;
bool isCountingSecondary = false;

unsigned long startTime = 0;
unsigned long currTime = millis();

void setup() {
  Serial.begin(115200);
  Wire.begin(33);
}

void loop() {
  float sensorValue = analogRead(A0);
  Serial.print("Analog Value =");
  Serial.println(sensorValue);
 
  float voltage = (sensorValue / 1024) * 5; //Arduino ADC resolution 0-1023
  // Serial.print("Voltage =");
  // Serial.print(voltage);
  // Serial.println(" V");

  float wind_speed = mapfloat(voltage, 0.4, 2, 0, 32.4);
  float speed_mph = ((wind_speed *3600)/1609.344);
  float speed_knot = ((wind_speed * 1.943844));
  Serial.print("Wind Speed =");
  Serial.print(wind_speed);
  Serial.println("m/s");
  // Serial.print(speed_mph);
  // Serial.println("mph");
  Serial.print(speed_knot);
  Serial.println("knots");
  Serial.println(" ");
  
  if (!isCounting && speed_knot > 35) { //Speed hits 35 for the first time
      isCounting = true; // flags begin feathering
      startTime = currTime;
  }

   if (currTime > startTime + 15000 && isCounting) { //Time threshold has been reached
      AnemData = 0; // Activates feather
      isCounting = false; // stops flag to begin feathering
   }

   if (speed_knot < 35 && currTime < startTime + 15000 && isCounting) { //It's been less than 15 seconds and our speed has reduced
      isCounting = false; // stops flag to begin feathering
   }

  if (speed_knot < 30 && AnemData == 0 && !isCountingSecondary) { //Speed dips below 30 for the first time after being at 35
      isCountingSecondary = true; // flags end feathering
      startTime = currTime;
  }

  if (currTime > startTime + 600000 && isCountingSecondary) { //Time threshold has been reached
      AnemData = 1; // Activates normal functioning
      isCountingSecondary = false; // stops flag to end feathering
  }

  if (speed_knot > 30 && currTime < startTime + 600000 && isCountingSecondary) { //It's been less than 10 minutes and our speed has increased
      isCountingSecondary = false; // stops flag to end feathering
  }

  Wire.onRequest(AnemData);
}

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
