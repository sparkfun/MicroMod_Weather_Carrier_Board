/*
 * MicroMod Weather Carrier Board VEML6075 Test
 * 
 * This sketch tests the VEML6075 IC. It takes UV
 * measurements.
 */

#include <SparkFun_VEML6075_Arduino_Library.h>

VEML6075 uv;

int STAT_LED = 19;

void setup() {
  Serial.begin(115200);
  Serial.println("MicroMod Weather Carrier Board Test - VEML6075");
  Serial.println();

  Wire.begin(); //Join I2C bus

  if (uv.begin() == false) {
    Serial.println("VEML6075 did not respond.");
    while(1);
  }

  pinMode(STAT_LED, OUTPUT);
}

void loop() {
  digitalWrite(STAT_LED, HIGH);
  Serial.println(String(uv.uva()) + ", " + String(uv.uvb()) + ", "+ String(uv.index()));
  Serial.println();

  digitalWrite(STAT_LED, LOW);
  delay(1000);
}
