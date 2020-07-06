/* 
 *  MicroMod Weather Carrier Board Soil Moisture Connector Test
 */

int STAT_LED = 19;
int moist_val = 0;  //Variable for storing moisture value
int soilPin = A32;  //Pin number that measures analog moisture signal
int soilPower = A16;  //Pin number that will power the soil moisture sensor

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
