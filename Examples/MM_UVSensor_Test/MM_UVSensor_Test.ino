/*
 * MicroMod Weather Carrier Board VEML6075 Example
 * 
 * This sketch tests the VEML6075 IC. It takes UV
 * measurements and communicates over I2C.
 * 
 * Priyanka Makin @ SparkX Labs
 * Original Creation Date: August 11, 2020
 * 
 * This code is Lemonadeware; if you see me (or any other SparkFun employee) at the
 * local, and you've found our code helpful, please buy us a round!
 * 
 * Hardware Connections:
 * Insert MicroMod processor board of your choice into the M.2 connector of the SparkFun Weather carrier
 *  Screw into place
 * Connect Weather carrier board to power using USB-C cable
 * 
 * Distributed as-is; no warranty is given.
 */

#include <SparkFun_VEML6075_Arduino_Library.h>

VEML6075 uv;

//LED pin
#if defined(ARDUINO_ARCH_APOLLO3)
int STAT_LED = 19;
#elif defined(ESP_PLATFORM)
int STAT_LED = 5;
#elif defined(ARDUINO_ARCH_SAMD)
int STAT_LED = 13;
#endif

void setup() {
  Serial.begin(115200);
  while(!Serial); //Wait for user to open serial monitor
  
  Serial.println("MicroMod Weather Carrier Board - VEML6075 Example");

  Wire.begin(); //Join I2C bus

  if (uv.begin() == false) {
    Serial.println("VEML6075 did not respond.");
    while(1);
  }

  pinMode(STAT_LED, OUTPUT);
  Serial.println("UVA, UVB, UV Index");
  Serial.println();

}

void loop() {
  digitalWrite(STAT_LED, HIGH);
  Serial.println(String(uv.uva()) + ", " + String(uv.uvb()) + ", "+ String(uv.index()));
  Serial.println();

  digitalWrite(STAT_LED, LOW);
  delay(1000);
}
