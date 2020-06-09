/*
  MicroMod Weather Carrier Board Test

  To Be Tested:
  Stat led
  BME280 - temp, pressure, humidity (I2C)
  VEML6075 - UV sensor (I2C)
  AS3935 - lightning detector (SPI)
  Soil moisture connector?
  Rain and Wind meters?
  uSD card?
*/

#include <Wire.h>
#include "SparkFunBME280.h"
#include <SparkFun_VEML6075_Arduino_Library.h>

BME280 tempSensor;
VEML6075 uv;

int STAT_LED = 19;

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
}

// the loop function runs over and over again forever
void loop() {
  Serial.println("BME280 Check");
  Serial.print("Temperature: ");
  Serial.println(tempSensor.readTempF(), 2);
  Serial.print("Humidity: ");
  Serial.println(tempSensor.readFloatHumidity(), 0);
  Serial.print("Pressure: ");
  Serial.println(tempSensor.readFloatPressure(), 0);
  Serial.print("Altitude: ");
  Serial.println(tempSensor.readFloatAltitudeFeet(), 1);
  Serial.println();

  Serial.println("VEML6075 Check");
  Serial.println(String(uv.uva()) + ", " + String(uv.uvb()) + ", "+ String(uv.index()));
  Serial.println();
 
  digitalWrite(STAT_LED, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(STAT_LED, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
}
