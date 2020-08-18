/*
  MicroMod Weather Carrier Board Test

  To Be Tested:
  DONE: Stat led
  DONE: BME280 - temp, pressure, humidity (I2C)
  DONE: VEML6075 - UV sensor (I2C)
  DONE: AS3935 - lightning detector (SPI) -- tested in separate sketch
  DONE: Soil moisture connector
  DONE: Wind Meter
  DONE: Rain Meter
  DONE: uSD card
*/

#include <Wire.h>
#include <SPI.h>
#include "SparkFunBME280.h"
#include <SparkFun_VEML6075_Arduino_Library.h>
#include <SparkFun_Alphanumeric_Display.h>  

BME280 tempSensor;
VEML6075 uv;
HT16K33 display;  //For testing the qwiic connector

#if defined(ARDUINO_ARCH_APOLLO3)
int STAT_LED = 19;
int soilPin = 32;  //Pin number that measures analog moisture signal
int soilPower = 16;  //Pin number that will power the soil moisture sensor
int WSPEED = 0; //Digital I/O pin for wind speed
int WDIR = 35; //Analog pin for wind direction
int RAIN = 1;   //Digital I/O pin for rain fall
#elif defined(ESP_PLATFORM)
int STAT_LED = 5;
int soilPin = 34;
int soilPower = 4;
int WSPEED = 23;
int WDIR = 35;
int RAIN = 27;
#elif defined(ARDUINO_ARCH_SAMD)
int STAT_LED = 13;
int soilPin = A0;
int soilPower = 2;
int WSPEED = 0;
int WDIR = A1;
int RAIN = 1;
#endif

int moist_val = 0;  //Variable for storing moisture value
volatile bool rain_flag = false;
volatile bool wind_flag = false;

//Function is called every time the rain bucket tips
void rainIRQ()
{
  rain_flag = true;
}

//Function is called when the magnet in the anemometer is activated
void wspeedIRQ()
{
  wind_flag = true;
}

void setup() {
  Serial.begin(115200);
  Serial.println("MicroMod Weather Carrier Board Test");
  Serial.println();

  Wire.begin();

  if (tempSensor.beginI2C() == false) { //Begin communication over I2C
    Serial.println("BME280 did not respond.");
    while(1); //Freeze
  }
  if (uv.begin() == false) {
    Serial.println("VEML6075 did not respond.");
    while(1);
  }
  
  pinMode(STAT_LED, OUTPUT);
  pinMode(soilPower, OUTPUT);
  digitalWrite(soilPower, LOW);

  //Initialization for weather meter
  pinMode(WSPEED, INPUT_PULLUP);  //Input from wind meters windspeed sensor
  pinMode(RAIN, INPUT_PULLUP);    //Input from wind meters rain gauge sensor
  //attach external interrupt pins to IRQ functions
  attachInterrupt(digitalPinToInterrupt(RAIN), rainIRQ, FALLING);
  attachInterrupt(digitalPinToInterrupt(WSPEED), wspeedIRQ, FALLING);
  //turn on interrupts
  interrupts();
}

// the loop function runs over and over again forever
void loop() {
  Serial.println();
  Serial.print("Temperature: ");
  Serial.println(tempSensor.readTempF(), 2);
  Serial.print("Humidity: ");
  Serial.println(tempSensor.readFloatHumidity(), 0);
  Serial.print("Pressure: ");
  Serial.println(tempSensor.readFloatPressure(), 0);
  Serial.print("Altitude: ");
  Serial.println(tempSensor.readFloatAltitudeFeet(), 1);

  Serial.print("UV A, B, index: ");
  Serial.println(String(uv.uva()) + ", " + String(uv.uvb()) + ", "+ String(uv.index()));

  Serial.print("Soil Moisture = ");
  Serial.println(readSoil());

  Serial.print("Wind direction: ");
  Serial.print(get_wind_direction());
  Serial.println(" degrees");
  //Check interrupt flags
  if (rain_flag == true){
    Serial.println("Rain click!");
    rain_flag = false;
  }
  if (wind_flag == true){
    Serial.println("Wind click!");
    wind_flag = false;
  }
  
  digitalWrite(STAT_LED, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(STAT_LED, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second

  delay(2000);
}

int readSoil() {
  //Power Senor
  digitalWrite(soilPower, HIGH);
  delay(10);
  moist_val = analogRead(soilPin);  //Read the SIG value from sensor
  digitalWrite(soilPower, LOW); //Turn the sensor off
  return moist_val; //Return current moisture value
}

int get_wind_direction()
{
  unsigned int adc;
  adc = analogRead(WDIR); //get the current readings from the sensor

  if (adc < 380) return (113);
  if (adc < 393) return (68);
  if (adc < 414) return (90);
  if (adc < 456) return (158);
  if (adc < 508) return (135);
  if (adc < 551) return (203);
  if (adc < 615) return (180);
  if (adc < 680) return (23);
  if (adc < 746) return (45);
  if (adc < 801) return (248);
  if (adc < 833) return (225);
  if (adc < 878) return (338);
  if (adc < 913) return (0);
  if (adc < 940) return (293);
  if (adc < 967) return (315);
  if (adc < 990) return (270);
  return (-1);
}
