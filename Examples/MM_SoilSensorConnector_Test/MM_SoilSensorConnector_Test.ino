/* 
 *  MicroMod Weather Carrier Board Soil Moisture Connector Test
 */

#if defined(ARDUINO_ARCH_APOLLO3)
int STAT_LED = 19;
int soilPin = A32;  //Pin number that measures analog moisture signal
int soilPower = A16;  //Pin number that will power the soil moisture sensor
#elif defined(ESP_PLATFORM)
int STAT_LED = 5;
int soilPin = 34;
int soilPower = 4;
#elif defined(ARDUINO_ARCH_SAMD)
int STAT_LED = 13;
int soilPin = A2;
int soilPower = 2;
#endif

int moist_val = 0;  //Variable for storing moisture value

void setup() {
  Serial.begin(115200);
  Serial.println("MicroMod Weather Carrier Board Test - Soil Moisture Connector");
  Serial.println();

  pinMode(soilPower, OUTPUT);
  digitalWrite(soilPower, LOW);

}

void loop() {
  digitalWrite(STAT_LED, HIGH);
  Serial.print("Soil Moisture = ");
  Serial.println(readSoil());
  Serial.println();

  digitalWrite(STAT_LED, LOW);
  delay(1000);
}

int readSoil() {
  //Power the sensor
  digitalWrite(soilPower, HIGH);
  delay(10);
  moist_val = analogRead(soilPin);  //Read the SIG value from sensor
  digitalWrite(soilPower, LOW); //Turn the sensor off
  return moist_val; //Return current moisture value
}
