#include <Adafruit_GPS.h>

// what's the name of the hardware serial port?
#define GPSSerial Serial1
#define TransmitActuator Serial2 // RX = 17, TX = 16
#define TransmitDCMotor Serial3 // RX = 15, TX = 14
// Connect to the GPS on the hardware port
Adafruit_GPS GPS(&GPSSerial);

// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences
#define GPSECHO false

uint32_t timer = millis();

    double rad_to_deg = 180/PI;
    double fract_year_rad = 0;
    double fract_year_deg = 0;
    double day_of_year = 0;
    double day = 0;
    double month = 0;
    double year = 0;
    double hour = 0;
    double minute = 0;
    double sec = 0;
    double long_;
    double lat_deg;
     //double long_rad = long_ / rad_to_deg;
    double lat_rad;
    double eqtime = 0;
    double decl_rad = 0;
    double decl_deg = 0;
    double off_set = 0;
    double true_solar_time = 0;
    double time_zone = -5;
    double Solar_Hour_Angle_deg = 0;
    double Solar_Hour_Angle_rad = 0;
    double Zenith_rad = 0;
    double Zenith_deg = 0;
    double Elevation_deg = 0;
    double Elevation_rad = 0;
    double Azimuth_rad = 0;
    double Azimuth_deg = 0;   

void setup()
{
  //while (!Serial);  // uncomment to have the sketch wait until Serial is ready

  // connect at 115200 so we can read the GPS fast enough and echo without dropping chars
  // also spit it out
  Serial.begin(115200);
  Serial.println("Adafruit GPS Starting up...");
  // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  GPS.begin(9600);
  TransmitActuator.begin(115200);
  TransmitDCMotor.begin(115200);
  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
  // the parser doesn't care about other sentences at this time
  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update rate
  // For the parsing code to work nicely and have time to sort thru the data, and
  // print it out we don't suggest using anything higher than 1 Hz

  // Request updates on antenna status, comment out to keep quiet
  GPS.sendCommand(PGCMD_ANTENNA);

  delay(1000);

  // Ask for firmware version
  GPSSerial.println(PMTK_Q_RELEASE);
}

void loop() // run over and over again
{
  // read data from the GPS in the 'main loop'
  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
  if (GPSECHO)
    if (c) Serial.print(c);
  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences!
    // so be very wary if using OUTPUT_ALLDATA and trying to print out data
    //Serial.print(GPS.lastNMEA()); // this also sets the newNMEAreceived() flag to false
    if (!GPS.parse(GPS.lastNMEA())) // this also sets the newNMEAreceived() flag to false
      return; // we can fail to parse a sentence in which case we should just wait for another
  }

  // approximately every 2 seconds or so, print out the current stats
  if (millis() - timer > 2000) {
    timer = millis(); // reset the timer
    Serial.print("\nTime: ");
    if (GPS.hour < 10) { Serial.print('0'); }
    Serial.print(GPS.hour, DEC); Serial.print(':');
    if (GPS.minute < 10) { Serial.print('0'); }
    Serial.print(GPS.minute, DEC); Serial.print(':');
    if (GPS.seconds < 10) { Serial.print('0'); }
    Serial.print(GPS.seconds, DEC); Serial.print('.');
    if (GPS.milliseconds < 10) {
      Serial.print("00");
    } else if (GPS.milliseconds > 9 && GPS.milliseconds < 100) {
      Serial.print("0");
    }
    Serial.println(GPS.milliseconds);
    Serial.print("Date: ");
    Serial.print(GPS.day, DEC); Serial.print('/');
    Serial.print(GPS.month, DEC); Serial.print("/20");
    Serial.println(GPS.year, DEC);
    Serial.print("Fix: "); Serial.print((int)GPS.fix);
    Serial.print(" quality: "); Serial.println((int)GPS.fixquality);
    if (GPS.fix) {
      Serial.print("Location: ");
      Serial.print(GPS.latitude, 4); Serial.print(GPS.lat);
      Serial.print(", ");
      Serial.print(GPS.longitude, 4); Serial.println(GPS.lon);
      Serial.println("Location in Degrees");
      Serial.print(GPS.latitudeDegrees, 8);
      Serial.print(", ");
      Serial.println(GPS.longitudeDegrees, 8);
      //Serial.print("Speed (knots): "); Serial.println(GPS.speed);
      //Serial.print("Angle: "); Serial.println(GPS.angle);
      //Serial.print("Altitude: "); Serial.println(GPS.altitude);
      Serial.print("Satellites: "); Serial.println((int)GPS.satellites);
      Serial.print("Antenna status: "); Serial.println((int)GPS.antenna);
    }
    
    //Comment out the following to run specific time tests
    day = calculateDayOfYear(GPS.day, GPS.month, GPS.year+2000);
    hour = GPS.hour + time_zone;
    if (GPS.hour >= 0 && GPS.hour <= 4)
    {
      hour += 24;
      day -= 1;
    }
  
    fract_year_rad = ((2*PI)/365)*(day - 1 + ((hour-12)/24));
   
    fract_year_deg = fract_year_rad * rad_to_deg;
        
    eqtime = (229.18)*((.000075) + (.001868*cos(fract_year_rad)) - (.032077*sin(fract_year_rad))
    - (0.014615*cos(2*fract_year_rad)) - (0.040849*sin(2*fract_year_rad)));

    decl_rad = (0.006918) - (0.399912*cos(fract_year_rad)) + (0.070257*sin(fract_year_rad)) 
    - (0.006758*cos(2*fract_year_rad)) + (0.000907*sin(2*fract_year_rad))
    - (0.002697*cos(3*fract_year_rad)) + (0.00148*sin(3*fract_year_rad));
    
    decl_deg = decl_rad * rad_to_deg;
    
    long_ = GPS.longitudeDegrees;
    lat_deg = GPS.latitudeDegrees;    

    lat_rad = lat_deg / rad_to_deg;
    
    off_set = eqtime + (4*long_) - (60*time_zone);
    
    minute = GPS.minute;

    sec = GPS.seconds;

    true_solar_time = (hour*60) + minute + (sec/60.0) + off_set;

    Solar_Hour_Angle_deg = (true_solar_time/4.0)-180;

    Solar_Hour_Angle_rad = (Solar_Hour_Angle_deg)/rad_to_deg;
    
    Zenith_rad = acos((sin(lat_rad)*sin(decl_rad)) + (cos(lat_rad)*cos(decl_rad)*cos(Solar_Hour_Angle_rad)));
    
    Zenith_deg = Zenith_rad*rad_to_deg;
    
    if (hour < 20 && hour > 5) { //if between 5 and 20, function as normal
        Elevation_deg = 90 - Zenith_deg;
    } else { //if outside 5 to 20, go to 0
        Elevation_deg = 0;
    }
    Serial.print("Elevation_deg: ");
    Serial.println(Elevation_deg);
    TransmitActuator.write(Elevation_deg);

    
    if (hour >= 12) {
        Azimuth_rad = -(acos(-((sin(lat_rad)*cos(Zenith_rad)-sin(decl_rad))/(cos(lat_rad)*sin(Zenith_rad))))) + (2*PI);
    } else if (hour < 12) {
        Azimuth_rad = acos((sin(decl_rad) - (cos(Zenith_rad)*sin(lat_rad)))/(sin(Zenith_rad)*cos(lat_rad)));
    }
    //Serial.print("Azimuth_rad: ");
    //Serial.println(Azimuth_rad);
    if (hour < 20 && hour > 5) { //if between 5 and 20, function as normal
        Azimuth_deg = (Azimuth_rad * rad_to_deg);
    } else { //if outside of 5 to 20, go to 0
        Azimuth_deg = 0;
    }
    Serial.print("Azimuth_deg: ");
    Serial.println(Azimuth_deg);
    TransmitDCMotor.write(Azimuth_deg);
    Serial.print("GPS Latitude: ");
    Serial.println(GPS.latitudeDegrees);

    Serial.print("GPS Longitude: ");
    Serial.println(GPS.longitudeDegrees);

//    Serial.print("Hour: ");
//    Serial.println(hour);
//    Serial.print("GPS Hour: ");
//    Serial.println(GPS.hour);
//
//    Serial.print("Minute: ");
//    Serial.println(minute);
//    Serial.print("GPS minute: ");
//    Serial.println(GPS.minute);
//
//    Serial.print("Seconds: ");
//    Serial.println(sec);
//    Serial.print("GPS seconds: ");
//    Serial.println(GPS.seconds);
//
//    Serial.print("GPS year: ");
//    Serial.println(GPS.year);
//    Serial.print("GPS Month: ");
//    Serial.println(GPS.month);
//    Serial.print("GPS Day: ");
//    Serial.println(GPS.day);
//    Serial.print("Day: ");
//    Serial.println(day);
    
  }
}

int calculateDayOfYear(int day, int month, int year) {
  
  // Given a day, month, and year (4 digit), returns 
  // the day of year. Errors return 999.
  
  int daysInMonth[] = {31,28,31,30,31,30,31,31,30,31,30,31};
  
  // Verify we got a 4-digit year
  if (year < 1000) {
    return 999;
  }
  
  // Check if it is a leap year, this is confusing business
  // See: https://support.microsoft.com/en-us/kb/214019
  if (year%4  == 0) {
    if (year%100 != 0) {
      daysInMonth[1] = 29;
    }
    else {
      if (year%400 == 0) {
        daysInMonth[1] = 29;
      }
    }
   }

  // Make sure we are on a valid day of the month
  if (day < 1) 
  {
    return 999;
  } else if (day > daysInMonth[month-1]) {
    return 999;
  }
  
  int doy = 0;
  for (int i = 0; i < month - 1; i++) {
    doy += daysInMonth[i];
  }
  
  doy += day;
  return doy;
}
