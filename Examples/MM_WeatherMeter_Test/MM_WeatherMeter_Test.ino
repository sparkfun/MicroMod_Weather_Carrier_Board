/*
 * MicroMod Weather Carrier Board Weather Meter Example
 * 
 * This sketch tests the RJ11 connectors and changes in the 
 * wind speed, wind direction, and rain meters.
 * 
 * Priyanka Makin @ SparkX Labs
 * Original Creation Date: August 11, 2020
 * 
 * This code is Lemonadeware; of you see me (or any other SparkFun employee) at the
 * local, and you've found our code helpful, please buy us a round!
 * 
 * Hardware Connections:
 * Insert MicroMod processor board of your choice into the M.2 connector of the SparkFun Weather carrier
 *  Screw into place
 * Connect Weather carrier board to power using USB-C cable
 * Connect both wind and rain meters to Weather carrier using the RJ11 connectors
 * 
 * Distributed as-is; no warranty is given.
 */

#if defined(ARDUINO_ARCH_APOLLO3)
int WSPEED = 0; //Digital I/O pin for wind speed
int WDIR = 35; //Analog pin for wind direction
int RAIN = 1; //Digital I/O pin for rain fall
#elif defined(ESP_PLATFORM)
int WSPEED = 23;
int WDIR = 35;
int RAIN = 27;
#elif defined(ARDUINO_ARCH_SAMD)
int WSPEED = 0;
int WDIR = A1;
int RAIN = 1;
#endif

volatile bool rain_flag = false;
volatile bool wind_flag = false;

//Function is called every time the rain bucket tips
void rainIRQ() {
  rain_flag = true;
}

//Function is called every time the rain bucket tips
void wspeedIRQ() {
  wind_flag = true;
}

void setup() {
  Serial.begin(115200);
  while (!Serial);  //Wait for user to open serial monitor
  
  Serial.println("MicroMod Weather Carrier Board Test - Weather Meter");
  Serial.println();

  //Initialization from weather meter
  pinMode(WSPEED, INPUT_PULLUP);  //Input from wind meters windspeed sensor
  pinMode(RAIN, INPUT_PULLUP);  //Input from wind meters rain gauge sensor
  //attach external interrupt pins to IRQ functions
  attachInterrupt(digitalPinToInterrupt(RAIN), rainIRQ, FALLING);
  attachInterrupt(digitalPinToInterrupt(WSPEED), wspeedIRQ, FALLING);
  //Turn on interrupts
  interrupts();
}

void loop() {
  Serial.print("Wind direction: ");
  Serial.print(get_wind_direction());
  Serial.println(" degrees");

  //Check interrupt flags
  if (rain_flag == true) {
    Serial.println("RAIN!");
    rain_flag = false;
  }
  if (wind_flag == true) {
    Serial.println("Wind click!");
    wind_flag = false;
  }
  delay(1000);
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
