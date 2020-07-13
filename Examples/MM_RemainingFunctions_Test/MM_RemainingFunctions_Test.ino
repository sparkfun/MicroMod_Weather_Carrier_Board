/*
 * MicroMod Weather Carrier Board Remaining Function Test
 * 
 * This sketch tests the following:
 * Stat LED
 * uSD card
 * PWM0/1
 * Qwiic Connector
 * VIN/3
 */

#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SparkFun_Alphanumeric_Display.h>

Sd2Card card;
SdVolume volume;
HT16K33 display;  //For testing the qwiic connector

int STAT_LED = 19;
const int SD_chipSelect = 23;
const int PIN_PWM0 = 44;
const int PIN_PWM1 = 45;

void setup() {
  Serial.begin(115200);
  Serial.println("MicroMod Weather Carrier Board Left Over Function Test");
  Serial.println();

  Wire.begin(); //Join I2C bus to test qwiic connector

  pinMode(STAT_LED, OUTPUT);

  testSD();
  testRemainingFunctions();
}

void loop() {
  digitalWrite(STAT_LED, HIGH);
  delay(1000);
  digitalWrite(STAT_LED, LOW);
  delay(1000);
}

//SD card test
void testSD() {
  Serial.println("Initializing SD card...");

  if (!card.init(SPI_HALF_SPEED, SD_chipSelect)) {
    Serial.println("initialization failed. Things to check: ");
    Serial.println("* is a card inserted?");
    Serial.println("* is your wiring correct?");
    Serial.println("* did you cahnge the chipSelect pin to match your module?");
    while(1);
  } else {
    Serial.println("Wiring is correct and a card is present.");
  }

  //Print the type of the card
  Serial.println();
  Serial.print("Card type:        ");
  switch (card.type()) {
    case SD_CARD_TYPE_SD1:
      Serial.println("SD1");
      break;
    case SD_CARD_TYPE_SD2:
      Serial.println("SD2");
      break;
    case SD_CARD_TYPE_SDHC:
      Serial.println("SDHC");
      break;
    default:
      Serial.println("Unknown");  
  }

  if (!volume.init(card)) {
    Serial.println("Could not find FAT16/FAT32 partition. \nMake sure you've formatted the card.");
    while(1);
  }

  //print the type and size of the first FAT-type volume
  uint32_t volumesize;
  Serial.print("Volume type is:     FAT");
  Serial.println(volume.fatType(), DEC);

  volumesize = volume.blocksPerCluster();
  volumesize *= volume.clusterCount();
  volumesize /= 2;
  volumesize /= 1024;
  Serial.print("Volume size (Gb):   ");
  Serial.println((float)volumesize / 1024.0);
}

void testRemainingFunctions() {
  Serial.println();
  //Test Qwiic connector
  Serial.println("Test Qwiic Connector");
  if (display.begin() == false) {
    Serial.println("Alphanumeric did not acknowledge! Freezing.");
    while(1);
  }
  Serial.println("Alphanumeric acknowledged.");
  display.print("Milk");
  Serial.println();

  //Test PWM0/1
  Serial.println("Test PWM0/1. Use multimeter to verify.");
  analogWrite(PIN_PWM0, 125);
//  analogWrite(PIN_PWM1, 125);
  Serial.println();

  //Test VIN/3
  Serial.println("VIN/3 Test");
  analogReadResolution(14);   //Change resolution to 14 bits
  int temp = analogRead(A31);
  float calc_temp = (float)(temp * 4.6)/16384;
  Serial.println(calc_temp);
}
