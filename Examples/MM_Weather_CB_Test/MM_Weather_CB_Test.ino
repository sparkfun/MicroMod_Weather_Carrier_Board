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
  Other
    PWM0/1 -- 1 doesn't work
    DONE: Qwiic Connector
    TX/RX 1/2 -- tested in separate sketch, need diff PB to test TX/RX2
    VIN/3 -- doesn't work
*/

#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include "SparkFunBME280.h"
#include <SparkFun_VEML6075_Arduino_Library.h>
#include <SparkFun_Alphanumeric_Display.h>  

BME280 tempSensor;
VEML6075 uv;
Sd2Card card;
SdVolume volume;
HT16K33 display;  //For testing the qwiic connector

int STAT_LED = 19;
int moist_val = 0;  //Variable for storing moisture value
int soilPin = A32;  //Pin number that measures analog moisture signal
int soilPower = A16;  //Pin number that will power the soil moisture sensor

int WSPEED = 1; //Digital I/O pin for wind speed
int WDIR = A35; //Analog pin for wind direction
int RAIN = 2;   //Digital I/O pin for rain fall
volatile bool rain_flag = false;
volatile bool wind_flag = false;

const int SD_chipSelect = 23;
//const int SD_chipSelect = CS; //Chip select pin for SD card
const int PIN_PWM0 = 44;
const int PIN_PWM1 = 45;


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

  testSD();
  testRemainingPeripherals();
}

// the loop function runs over and over again forever
void loop() {
  Serial.println();
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

  Serial.println("Soil Moisture Sensor Check");
  Serial.print("Soil Moisture = ");
  Serial.println(readSoil());

  Serial.println("Weather Meter Wind Direction Check");
  Serial.print("Wind direction: ");
  Serial.print(get_wind_direction());
  Serial.println(" degrees");
  //Check interrupt flags
  if (rain_flag == true){
    Serial.println("RRRRRAAAAIIIINNN!!!");
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

void testSD() {
  //SD Card test
  Serial.println("Initializing SD card...");
  
  if (!card.init(SPI_HALF_SPEED, SD_chipSelect)) {
    Serial.println("initialization failed. Things to check:");
    Serial.println("* is a card inserted?");
    Serial.println("* is your wiring correct?");
    Serial.println("* did you change the chipSelect pin to match your module?");
    while(1);
  } else {
    Serial.println("Wiring is correct and a card is present.");
  }

  //print the type of card
  Serial.println();
  Serial.print("Card type:          ");
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
    Serial.println("Could not find FAT16/FAT32 partition. \nMake sure you've formatted the card");
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
  Serial.print("Volume size (Gb): ");
  Serial.println((float)volumesize / 1024.0);
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

void testRemainingPeripherals() {
  Serial.println();
  //Test Qwiic Connector
  Serial.println("Test Qwiic Connector");
  if (display.begin() == false)
  {
    Serial.println("Alphanumeric did not acknowledge! Freezing.");
    while(1);
  }
  Serial.println("Alphanumeric acknowledged.");
  display.print("Milk");
  Serial.println();
  
  //Test PWM0/1
  Serial.println("Test PWM0/1. Use multimeter to verify.");
  analogWrite(PIN_PWM0, 125);
//  analogWrite(PIN_PWM1, 125); //TODO: this one doesn't work
  Serial.println();

  //Test VIN/3
  Serial.println("VIN/3 Test");
  analogReadResolution(14);
  int temp = analogRead(31);
//  temp = (temp * 3 * 2)/16384;
  Serial.println(temp);
}
