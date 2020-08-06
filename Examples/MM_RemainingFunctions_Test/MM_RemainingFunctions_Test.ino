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

#if defined(ARDUINO_ARCH_APOLLO3)
#define ANALOG_RESOLUTION 14
int STAT_LED = 19;
const int SD_chipSelect = 23;
const int PIN_PWM0 = 44;
const int PIN_PWM1 = 45;
const int PIN_VIN3 = A31;
#elif defined(ESP_PLATFORM)
#define ANALOG_RESOLUTION 12
int STAT_LED = 5;
const int SD_chipSelect = 13;
const int PIN_PWM0 = 18;
const int PIN_PWM1 = 19;
const int freq = 5000;
const int LEDC_CHANNEL_0 = 0;
const int LEDC_CHANNEL_1 = 1;
const int resolution = 8;
const int PIN_VIN3 = 39;
#endif

int maxADCValue = pow(2, ANALOG_RESOLUTION);

//Sd2Card card;
//SdVolume volume;
HT16K33 display;  //For testing the qwiic connector

void testRemainingFunctions() {
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
  Serial.println("Test PWM0/1. Use multimeter to verify voltage is 1.6V.");
  #if defined(ESP_PLATFORM)
  ledcWrite(LEDC_CHANNEL_0, 125);
  ledcWrite(LEDC_CHANNEL_1, 125);
  #else
  analogWrite(PIN_PWM0, 125);
  analogWrite(PIN_PWM1, 125);
  #endif
  Serial.println();

  //Test VIN/3
  Serial.println("VIN/3 Test. This is the input voltage/3. Should be 1.6V.");
  int temp = analogRead(PIN_VIN3);
  float correctionValue = 5.0;
  #if defined(ARDUINO_ARCH_APOLLO3)
  correctionValue = 4.6;
  #elif defined(ESP_PLATFORM)
  correctionValue = 3.3;
  #endif
  Serial.println(temp);
  float calc_temp = (float)(temp * correctionValue) / maxADCValue;
  #if defined(ESP_PLATFORM)
  //Need to add offset for ESP32.
  //See: https://i0.wp.com/randomnerdtutorials.com/wp-content/uploads/2019/05/ADC-non-linear-ESP32.png?w=768&ssl=1
  calc_temp = calc_temp + 0.15;
  #endif
  Serial.println(calc_temp);
}

void setup() {
  Serial.begin(115200);
  Serial.println("MicroMod Weather Carrier Board Left Over Function Test");
  Serial.println();

  Wire.begin(); //Join I2C bus to test qwiic connector

  pinMode(STAT_LED, OUTPUT);

  //Set up for PWM
  #if defined(ESP_PLATFORM)
  ledcSetup(LEDC_CHANNEL_0, freq, resolution);
  ledcSetup(LEDC_CHANNEL_1, freq, resolution);
  ledcAttachPin(PIN_PWM0, LEDC_CHANNEL_0);
  ledcAttachPin(PIN_PWM1, LEDC_CHANNEL_1);
  #endif

  analogReadResolution(ANALOG_RESOLUTION);

//  testSD();
  testRemainingFunctions();
}

void loop() {
  digitalWrite(STAT_LED, HIGH);
  delay(1000);
  digitalWrite(STAT_LED, LOW);
  delay(1000);
}

////SD card test
//void testSD() {
//  Serial.println("Initializing SD card...");
//
//  if (!card.init(SPI_HALF_SPEED, SD_chipSelect)) {
//    Serial.println("initialization failed. Things to check: ");
//    Serial.println("* is a card inserted?");
//    Serial.println("* is your wiring correct?");
//    Serial.println("* did you cahnge the chipSelect pin to match your module?");
//    while(1);
//  } else {
//    Serial.println("Wiring is correct and a card is present.");
//  }
//
//  //Print the type of the card
//  Serial.println();
//  Serial.print("Card type:        ");
//  switch (card.type()) {
//    case SD_CARD_TYPE_SD1:
//      Serial.println("SD1");
//      break;
//    case SD_CARD_TYPE_SD2:
//      Serial.println("SD2");
//      break;
//    case SD_CARD_TYPE_SDHC:
//      Serial.println("SDHC");
//      break;
//    default:
//      Serial.println("Unknown");  
//  }
//
//  if (!volume.init(card)) {
//    Serial.println("Could not find FAT16/FAT32 partition. \nMake sure you've formatted the card.");
//    while(1);
//  }
//
//  //print the type and size of the first FAT-type volume
//  uint32_t volumesize;
//  Serial.print("Volume type is:     FAT");
//  Serial.println(volume.fatType(), DEC);
//
//  volumesize = volume.blocksPerCluster();
//  volumesize *= volume.clusterCount();
//  volumesize /= 2;
//  volumesize /= 1024;
//  Serial.print("Volume size (Gb):   ");
//  Serial.println((float)volumesize / 1024.0);
//}
