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
#if defined(ESP_PLATFORM)
int LED_BUILTIN = 5;
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

  pinMode(LED_BUILTIN, OUTPUT);
  Serial.println("UVA, UVB, UV Index");
  Serial.println();

}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println(String(uv.uva()) + ", " + String(uv.uvb()) + ", "+ String(uv.index()));
  Serial.println();

  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
}
