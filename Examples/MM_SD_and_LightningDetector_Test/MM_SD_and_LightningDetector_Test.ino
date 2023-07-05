/*
 * MicroMod Weather Carrier Board SD Card and Lightning Detector Example
 * 
 * This example demonstrates how to detect lightning and record the data to an SD card
 * 
 * Priyanka Makin @ SparkX Labs
 * Based on code by Elias Santistevan
 * Original Creation Date: Jan 13, 2021
 * 
 * Hardware Connections:
 * Insert MicroMod processor board of your choice into the M.2 connector of the SparkFun Weather carrier
 *  Screw into place
 * Connect Weather carrier board to power using USB-C cable
 * Insert a compatible uSD card into the SD card slot
 * 
 * Distributed as-is; no warranty is given.
*/

#include <SPI.h>
#include <SD.h>
#include "SparkFun_AS3935.h"

File myFile;
SparkFun_AS3935 lightning;

int spiCS = G1;
int disturber = 6;
int noise = 3;
//#define OUTDOOR 0xE
//#define INDOOR 0x12

#if defined(ARDUINO_ARCH_APOLLO3)
const int chipSelect = CS;
#else
const int chipSelect = SS;
#endif

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  Serial.println("Initializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    while(1);
  }
  Serial.println("initialization done.");

  myFile = SD.open("test.txt", FILE_WRITE);
  if (myFile) {
    Serial.print("Writing to test.txt...");
    myFile.println("testing 1, 2, 3.");
    //close the file
    myFile.close();
    Serial.println("done.");
  } else {
    //if the file didn't open, print an error
    Serial.println("error opening test.txt");
  }

  //re-open the file for reading:
  myFile = SD.open("test.txt");
  if (myFile) {
    Serial.println("test.txt:");

    //Read from the file until there's nothing else in it:
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    //close the file
    myFile.close();
  } else {
    //if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }

  SPI.begin();
  if (lightning.beginSPI(spiCS, 2000000) == false) {
    Serial.println("Lightning Detector did not start up, freezing!");
    while(1);
  } else {
    Serial.println("Schmow-ZoW, Lightning Detector Ready!");
  }

  lightning.setIndoorOutdoor(INDOOR); 
  lightning.setNoiseLevel(noise); 
  lightning.watchdogThreshold(disturber);
  
  Serial.print("Watchdog threshold: ");
  Serial.println(lightning.readWatchdogThreshold());
  Serial.print("Noise level: ");
  Serial.println(lightning.readNoiseLevel());
  Serial.print("Indoor/outdoor: ");
  Serial.println(lightning.readIndoorOutdoor());  
}

void loop() {
  // put your main code here, to run repeatedly:

}
