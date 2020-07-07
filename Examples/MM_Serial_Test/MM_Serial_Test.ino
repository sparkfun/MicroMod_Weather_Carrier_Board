// GNSS Echo

void setup()
{
  Serial.begin(115200);
  Serial1.begin(115200);
//  Serial2.begin(115200);

  while (!Serial); // Wait for user to run python script or open serial monitor

  delay(100);

  Serial.println("This is Serial 0");
  Serial1.println("This is Serial 1");
//  Serial2.println("This is Serial 2");
}
     
void loop() // run over and over again
{
  while (Serial.available()) {
    char c = Serial.read();
    Serial1.write(c);
//    Serial2.write(c);
  }
  while (Serial1.available()) { //Checks Serial1 RX buffer
    char c = Serial1.read();
    Serial.write(c);  //Print to Serial Monitor
  }  
//  while (Serial2.available()) { //Checks Serial2 RX buffer
//    char c = Serial2.read();  
//    Serial.write(c);  //Print to Serial Monitor
//  }
}
