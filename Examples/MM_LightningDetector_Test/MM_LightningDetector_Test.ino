/*
 * MicroMod Weather Carrier Board Lightning Detector Example
 * 
 * This example demonstrates how to detect lightning. It uses the onboard interrupt pin.
 * 
 * Priyanka Makin @ SparkX Labs
 * Based on code by Elias Santistevan
 * Original Creation Date: August 11, 2020
 * 
 * Hardware Connections:
 * Insert MicroMod processor board of your choice into the M.2 connector of the SparkFun Weather carrier
 *  Screw into place
 * Connect Weather carrier board to power using USB-C cable
 * 
 * Distributed as-is; no warranty is given.
*/

#include <SPI.h>
#include <Wire.h>
#include "SparkFun_AS3935.h"

#define INDOOR 0x12 
#define OUTDOOR 0xE
#define LIGHTNING_INT 0x08
#define DISTURBER_INT 0x04
#define NOISE_INT 0x01

SparkFun_AS3935 lightning;

const int lightningInt = G3;
int spiCS = G1;

// This variable holds the number representing the lightning or non-lightning
// event issued by the lightning detector. 
int intVal = 0;
int noise = 2; // Value between 1-7 
int disturber = 2; // Value between 1-10

void setup()
{
  Serial.begin(115200); 
  while (!Serial);  //Wait for user to open serial monitor
  
  Serial.println("MicroMod Weather Carrier Board - AS3935 Lightning Detector Example"); 
  Serial.println();
  
  // When lightning is detected the interrupt pin goes HIGH.
  pinMode(lightningInt, INPUT); 

  SPI.begin();
  
  if(lightning.beginSPI(spiCS, 2000000) == false){ 
    Serial.println ("Lightning Detector did not start up, freezing!"); 
    while(1); 
  }
  else
    Serial.println("Schmow-ZoW, Lightning Detector Ready!");

  // The lightning detector defaults to an indoor setting at 
  // the cost of less sensitivity, if you plan on using this outdoors 
  // uncomment the following line:
//  lightning.setIndoorOutdoor(OUTDOOR); 
}

void loop()
{
  //This is for debugging
  Serial.println(digitalRead(lightningInt));
  
   // Hardware has alerted us to an event, now we read the interrupt register
  if(digitalRead(lightningInt) == HIGH){
    intVal = lightning.readInterruptReg();
    if(intVal == NOISE_INT){
      Serial.println("Noise."); 
      // Too much noise? Uncomment the code below, a higher number means better
      // noise rejection.
//      lightning.setNoiseLevel(noise); 
    }
    else if(intVal == DISTURBER_INT){
      Serial.println("Disturber."); 
      // Too many disturbers? Uncomment the code below, a higher number means better
      // disturber rejection.
//      lightning.watchdogThreshold(disturber);  
    }
    else if(intVal == LIGHTNING_INT){
      Serial.println("Lightning Strike Detected!"); 
      // Lightning! Now how far away is it? Distance estimation takes into
      // account any previously seen events in the last 15 seconds. 
      byte distance = lightning.distanceToStorm(); 
      Serial.print("Approximately: "); 
      Serial.print(distance); 
      Serial.println("km away!"); 
    }
    else {
      Serial.print("Unknown interrupt val: ");
      Serial.println(intVal);
    }
  }
  delay(100); // Slow it down.
}
