/*
 * MicroMod Weather Carrier Board Weather Meter Example
 * 
 * This sketch tests the RJ11 connectors and changes in the 
 * wind speed, wind direction, and rain meters.
 * 
 * Priyanka Makin @ SparkX Labs
 * Original Creation Date: August 11, 2020
 * 
 * Hardware Connections:
 * Insert MicroMod processor board of your choice into the M.2 connector of the SparkFun Weather carrier
 *  Screw into place
 * Connect Weather carrier board to power using USB-C cable
 * Connect both wind and rain meters to Weather carrier using the RJ11 connectors
 * 
 * Distributed as-is; no warranty is given.
 * 
 * Updated be Wes Furuya
 * 07/10/2023
 * Implemented "Weather Meter" Arduino library
 * Updated pins for other processor boards
 */


#include "SparkFun_Weather_Meter_Kit_Arduino_Library.h"  //http://librarymanager/All#SparkFun_Weather_Meter_Kit


//Hardware pin definitions
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#if defined(ESP_PLATFORM)
const byte WSPEED = 14;  //Digital I/O pin for wind speed
const byte WDIR = 35;  //Analog pin for wind direction
const byte RAIN = 27;  //Digital I/O pin for rain fall
#elif defined(ARDUINO_RASPBERRY_PI_PICO)
const byte WSPEED = 6;  //Digital I/O pin for wind speed
const byte WDIR = A1;  //Analog pin for wind direction
const byte RAIN = 7;  //Digital I/O pin for rain fall
#elif defined(ARDUINO_TEENSY_MICROMOD)
const byte WSPEED = 4;  //Digital I/O pin for wind speed
const byte WDIR = A1;  //Analog pin for wind direction
const byte RAIN = 4;  //Digital I/O pin for rain fall
#else
const byte WSPEED = D0;  //Digital I/O pin for wind speed
const byte WDIR = A1;  //Analog pin for wind direction
const byte RAIN = D1;  //Digital I/O pin for rain fall
#endif
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

//Global Variables
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
long lastSecond;  //The millis counter to see when a second rolls by

float wind_dir = 0;    // [degrees (Cardinal)]
float wind_speed = 0;  // [kph]
float rain = 0;        // [mm]
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

SFEWeatherMeterKit myweatherMeterKit(WDIR, WSPEED, RAIN);  // Create an instance of the weather meter kit



void setup() {
  Serial.begin(115200);
  Serial.println("Example - Weather Meter Kit");

  pinMode(LED_BUILTIN, OUTPUT);  //Status LED Blue

  // The weather meter kit library assumes a 12-bit ADC
  // Configuring a 10-bit ADC resolution for the ATmega328 (RedBoard/Uno)
  myweatherMeterKit.setADCResolutionBits(10);

  // Begin weather meter kit
  myweatherMeterKit.begin();

  lastSecond = millis();

  Serial.println("Begin data collection!");
}

void loop() {
  //Keep track of which minute it is
  if (millis() - lastSecond >= 1000) {
    digitalWrite(LED_BUILTIN, HIGH);  //Blink stat LED

    lastSecond += 1000;

    //Report all readings every second
    printWeather();
  }

  digitalWrite(LED_BUILTIN, LOW);  //Turn off stat LED

  delay(100);
}

//Calculates data from weather meter kit 
void calcWeather() {

  //Weather Meter Kit
  //Calc Wind
  wind_dir = myweatherMeterKit.getWindDirection();
  wind_speed = myweatherMeterKit.getWindSpeed();
  //Calc Rain
  rain = myweatherMeterKit.getTotalRainfall();
}

//Prints the various variables directly to the port
//I don't like the way this function is written but Arduino doesn't support floats under sprintf
void printWeather() {
  calcWeather();  //Go calc all the various sensors

  Serial.println();
  Serial.print("wind direction= ");
  Serial.print(wind_dir, 1);
  Serial.print(" deg, wind speed= ");
  Serial.print(wind_speed, 1);
  Serial.print(" kph, total rain= ");
  Serial.print(rain, 1);
  Serial.println(" mm");
}
