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

#if defined(ESP_PLATFORM)
int LED_BUILTIN = 5;
//int A0 = 34;
int G0 = 4;
#elif defined(ARDUINO_ARCH_SAMD)
int G0 = 2;
#endif

int soilPin = A0;  //Pin number that measures analog moisture signal
int soilPower = G0;  //Pin number that will power the soil moisture sensor

void setup() {
  Serial.begin(115200);
  while(!Serial); //Wait for user to open serial monitor
  
  Serial.println("MicroMod Weather Carrier Board - Soil Moisture Connector Example");
  Serial.println();

  pinMode(soilPower, OUTPUT);
  digitalWrite(soilPower, LOW);

}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.print("Soil Moisture = ");
  Serial.println(readSoil());
  Serial.println();

  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
}

int readSoil() {
  int moistVal = 0;  //Variable for storing moisture value
  //Power the sensor
  digitalWrite(soilPower, HIGH);
  delay(10);
  moistVal = analogRead(soilPin);   //Read the SIG value from sensor. Moisture value depends on PB's operating voltage and ADC resolution
                                    //Check out the SparkFun Soil Moisture Sensor hookup guide for tips on calibrating your sensor
                                    //https://learn.sparkfun.com/tutorials/soil-moisture-sensor-hookup-guide?_ga=2.27184827.782829043.1597935536-2045279763.156684911
  digitalWrite(soilPower, LOW); //Turn the sensor off
  return moistVal; //Return current moisture value
}
