/* 
 *  MicroMod Weather Carrier Board Soil Moisture Connector Example
 *  
 *  This sketch tests the the connector and reading the analog values
 *  of the soil mosture sensor.
 *  
 *  Priyanka Makin @ SparkX Labs
 *  Original Creation Date: August 11, 2020
 *  
 * This code is Lemonadeware; if you see me (or any other SparkFun employee) at the
 * local, and you've found our code helpful, please buy us a round!
 * 
 *  Hardware connections:
 *  Insert MicroMod processor board of your choice into the M.2 connector of the SparkFun Weather carrier
 *    Screw into place
 *  Connect Weather carrier board to power using USB-C cable
 *  Latch the three wires of the SparkFun Soil Moisture Sensor (SEN-13637) to Weather carrier
 *  
 *  Distributed as-is; no warranty is given.
 */

#if defined(ARDUINO_ARCH_APOLLO3)
int STAT_LED = 19;
int soilPin = 32;  //Pin number that measures analog moisture signal
int soilPower = 16;  //Pin number that will power the soil moisture sensor
#elif defined(ESP_PLATFORM)
int STAT_LED = 5;
int soilPin = 34;
int soilPower = 4;
#elif defined(ARDUINO_ARCH_SAMD)
int STAT_LED = 13;
int soilPin = A0;
int soilPower = 2;
#endif

int moist_val = 0;  //Variable for storing moisture value

void setup() {
  Serial.begin(115200);
  while(!Serial); //Wait for user to open serial monitor
  
  Serial.println("MicroMod Weather Carrier Board - Soil Moisture Connector Example");
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
