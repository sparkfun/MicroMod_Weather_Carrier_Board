/*
 * MicroMod Weather Carrier Board BME280 Test
 * 
 * This sketch tests the BME280 IC. It communicates over
 * I2C and measures temperature, pressure, and humidity
 */
 
#include <Wire.h>
#include "SparkFunBME280.h"

BME280 tempSensor;

int STAT_LED = 19;

void setup() {
  Serial.begin(115200);
  Serial.println("MicroMod Weather Carrier Board - BME280 Test");
  Serial.println();

  Wire.begin(); //Join I2C bus

  if (tempSensor.begin() == false) { //Connect to BME280
    Serial.println("BME280 did not respond.");
    while(1); //Freeze
  }

  pinMode(STAT_LED, OUTPUT);
}

void loop() {
  digitalWrite(STAT_LED, HIGH);
  Serial.print("Temperature: ");
  Serial.println(tempSensor.readTempF(), 2);
  Serial.print("Humidity: ");
  Serial.println(tempSensor.readFloatHumidity(), 0);
  Serial.print("Pressure: ");
  Serial.println(tempSensor.readFloatPressure(), 0);
  Serial.print("Altitude: ");
  Serial.println(tempSensor.readFloatAltitudeFeet(), 1);
  Serial.println();

  digitalWrite(STAT_LED, LOW);
  delay(1000);
}
