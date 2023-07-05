/*
 * MicroMod Weather Carrier Board BME280 Example
 * 
 * This sketch tests the BME280 IC. It communicates over
 * I2C and measures temperature, pressure, and humidity
 * 
 * Priyanka Makin @ SparkX Labs
 * Original Creation Date: August 11, 2020
 * 
 * Hardware connections:
 * Insert MicroMod processor board of your choice into the M.2 connector of the SparkFun Weather carrier 
 *  Screw into place
 * Connect Weather carrier board to power using USB-C cable
 * 
 * Distributed as-is; no warranty is given.
 */
 
#include <Wire.h>
#include "SparkFunBME280.h"

BME280 tempSensor;

//LED pin
#if defined(ESP_PLATFORM)
int LED_BUILTIN = 5;
#endif

void setup() {
  Serial.begin(115200);
  while (!Serial);  //Wait for user to open serial monitor
  
  Serial.println("MicroMod Weather Carrier Board - BME280 Example");
  Serial.println();

  Wire.begin(); //Join I2C bus

  if (tempSensor.begin() == false) { //Connect to BME280
    Serial.println("BME280 did not respond.");
    while(1); //Freeze
  }

  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.print("Temperature: ");
  Serial.println(tempSensor.readTempF(), 2);
  Serial.print("Humidity: ");
  Serial.println(tempSensor.readFloatHumidity(), 0);
  Serial.print("Pressure: ");
  Serial.println(tempSensor.readFloatPressure(), 0);
  Serial.print("Altitude: ");
  Serial.println(tempSensor.readFloatAltitudeFeet(), 1);
  Serial.println();

  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
}
