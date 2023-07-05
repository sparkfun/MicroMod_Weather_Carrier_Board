/*
  Weather Shield Example
  By: Nathan Seidle
  SparkFun Electronics
  Date: November 16th, 2013
  License: MIT

  Much of this is based on Mike Grusin's USB Weather Board code: https://www.sparkfun.com/products/10586

  This is a more advanced example of how to utilize every aspect of the weather shield. See the basic
  example if you're just getting started.

  This code reads all the various sensors (wind speed, direction, rain gauge, humidty, pressure, light, batt_lvl)
  and reports it over the serial comm port. This can be easily routed to an datalogger (such as OpenLog) or
  a wireless transmitter (such as Electric Imp).

  Measurements are reported once a second but windspeed and rain gauge are tied to interrupts that are
  calcualted at each report.

  This example code assumes the GPS module is not used.


  Updated by Joel Bartlett
  03/02/2017
  Removed HTU21D code and replaced with Si7021

*/

#include <Wire.h> //I2C needed for sensors

//Hardware pin definitions
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#if defined(ESP_PLATFORM)
int D0 = 23;
int A1 = 35;
int D1 = 27;
#elif defined(ARDUINO_ARCH_SAMD)
int D0 = 0;
int D1 = 1;
#endif

int WSPEED = D0;  //Digital I/O pin for wind speed
int WDIR = A1;  //Analog pin for wind direction
int RAIN = D1;  //Digital I/O pin for rain fall
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

//Global Variables
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
long lastSecond; //The millis counter to see when a second rolls by
byte seconds; //When it hits 60, increase the current minute
byte seconds_2m; //Keeps track of the "wind speed/dir avg" over last 2 minutes array of data
byte minutes; //Keeps track of where we are in various arrays of data
byte minutes_10m; //Keeps track of where we are in wind gust/dir over last 10 minutes array of data

long lastWindCheck = 0;
volatile long lastWindIRQ = 0;
volatile byte windClicks = 0;

//We need to keep track of the following variables:
//Wind speed/dir each update (no storage)
//Wind gust/dir over the day (no storage)
//Wind speed/dir, avg over 2 minutes (store 1 per second)
//Wind gust/dir over last 10 minutes (store 1 per minute)
//Rain over the past hour (store 1 per minute)
//Total rain over date (store one per day)

byte windspdavg[120]; //120 bytes to keep track of 2 minute average

#define WIND_DIR_AVG_SIZE 120
int winddiravg[WIND_DIR_AVG_SIZE]; //120 ints to keep track of 2 minute average
float windgust_10m[10]; //10 floats to keep track of 10 minute max
int windgustdirection_10m[10]; //10 ints to keep track of 10 minute max
volatile float rainHour[60]; //60 floating numbers to keep track of 60 minutes of rain

//These are all the weather values that wunderground expects:
int winddir = 0; // [0-360 instantaneous wind direction]
float windspeedmph = 0; // [mph instantaneous wind speed]
float windgustmph = 0; // [mph current wind gust, using software specific time period]
int windgustdir = 0; // [0-360 using software specific time period]
float windspdmph_avg2m = 0; // [mph 2 minute average wind speed mph]
int winddir_avg2m = 0; // [0-360 2 minute average wind direction]
float windgustmph_10m = 0; // [mph past 10 minutes wind gust mph ]
int windgustdir_10m = 0; // [0-360 past 10 minutes wind gust direction]
float rainin = 0; // [rain inches over the past hour)] -- the accumulated rainfall in the past 60 min
volatile float dailyrainin = 0; // [rain inches so far today in local time]

// volatiles are subject to modification by IRQs
volatile unsigned long raintime, rainlast, raininterval, rain;

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

//Interrupt routines (these are called by the hardware interrupts, not by the main code)
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void rainIRQ()
// Count rain gauge bucket tips as they occur
// Activated by the magnet and reed switch in the rain gauge, attached to input D2
{
  raintime = millis(); // grab current time
  raininterval = raintime - rainlast; // calculate interval between this and last event

  if (raininterval > 10) // ignore switch-bounce glitches less than 10mS after initial edge
  {
    dailyrainin += 0.011; //Each dump is 0.011" of water
    rainHour[minutes] += 0.011; //Increase this minute's amount of rain

    rainlast = raintime; // set up for next event
  }
}

void wspeedIRQ()
// Activated by the magnet in the anemometer (2 ticks per rotation), attached to input D3
{
  if (millis() - lastWindIRQ > 10) // Ignore switch-bounce glitches less than 10ms (142MPH max reading) after the reed switch closes
  {
    lastWindIRQ = millis(); //Grab the current time
    windClicks++; //There is 1.492MPH for each click per second.
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.println("Weather Shield Example");

  pinMode(WSPEED, INPUT_PULLUP); // input from wind meters windspeed sensor
  pinMode(RAIN, INPUT_PULLUP); // input from wind meters rain gauge sensor

  seconds = 0;
  lastSecond = millis();

  // attach external interrupt pins to IRQ functions
  attachInterrupt(RAIN, rainIRQ, FALLING);
  attachInterrupt(WSPEED, wspeedIRQ, FALLING);

  // turn on interrupts
  interrupts();

  Serial.println("Weather Shield online!");

}

void loop()
{
  //Keep track of which minute it is
  if (millis() - lastSecond >= 1000)
  {
    digitalWrite(LED_BUILTIN, HIGH); //Blink stat LED

    lastSecond += 1000;

    //Take a speed and direction reading every second for 2 minute average
    if (++seconds_2m > 119) seconds_2m = 0;

    //Calc the wind speed and direction every second for 120 second to get 2 minute average
    float currentSpeed = get_wind_speed();
    windspeedmph = currentSpeed;//update global variable for windspeed when using the printWeather() function
    //float currentSpeed = random(5); //For testing
    int currentDirection = get_wind_direction();
    windspdavg[seconds_2m] = (int)currentSpeed;
    winddiravg[seconds_2m] = currentDirection;
    //if(seconds_2m % 10 == 0) displayArrays(); //For testing

    //Check to see if this is a gust for the minute
    if (currentSpeed > windgust_10m[minutes_10m])
    {
      windgust_10m[minutes_10m] = currentSpeed;
      windgustdirection_10m[minutes_10m] = currentDirection;
    }

    //Check to see if this is a gust for the day
    if (currentSpeed > windgustmph)
    {
      windgustmph = currentSpeed;
      windgustdir = currentDirection;
    }

    if (++seconds > 59)
    {
      seconds = 0;

      if (++minutes > 59) minutes = 0;
      if (++minutes_10m > 9) minutes_10m = 0;

      rainHour[minutes] = 0; //Zero out this minute's rainfall amount
      windgust_10m[minutes_10m] = 0; //Zero out this minute's gust
    }

    //Report all readings every second
    printWeather();

    digitalWrite(LED_BUILTIN, LOW); //Turn off stat LED
  }

  delay(100);
}

//Calculates each of the variables that wunderground is expecting
void calcWeather()
{
  //Calc winddir
  winddir = get_wind_direction();

  //Calc windspeed
  //windspeedmph = get_wind_speed(); //This is calculated in the main loop on line 185

  //Calc windgustmph
  //Calc windgustdir
  //These are calculated in the main loop

  //Calc windspdmph_avg2m
  float temp = 0;
  for (int i = 0 ; i < 120 ; i++)
    temp += windspdavg[i];
  temp /= 120.0;
  windspdmph_avg2m = temp;

  //Calc winddir_avg2m, Wind Direction
  //You can't just take the average. Google "mean of circular quantities" for more info
  //We will use the Mitsuta method because it doesn't require trig functions
  //And because it sounds cool.
  //Based on: http://abelian.org/vlf/bearings.html
  //Based on: http://stackoverflow.com/questions/1813483/averaging-angles-again
  long sum = winddiravg[0];
  int D = winddiravg[0];
  for (int i = 1 ; i < WIND_DIR_AVG_SIZE ; i++)
  {
    int delta = winddiravg[i] - D;

    if (delta < -180)
      D += delta + 360;
    else if (delta > 180)
      D += delta - 360;
    else
      D += delta;

    sum += D;
  }
  winddir_avg2m = sum / WIND_DIR_AVG_SIZE;
  if (winddir_avg2m >= 360) winddir_avg2m -= 360;
  if (winddir_avg2m < 0) winddir_avg2m += 360;

  //Calc windgustmph_10m
  //Calc windgustdir_10m
  //Find the largest windgust in the last 10 minutes
  windgustmph_10m = 0;
  windgustdir_10m = 0;
  //Step through the 10 minutes
  for (int i = 0; i < 10 ; i++)
  {
    if (windgust_10m[i] > windgustmph_10m)
    {
      windgustmph_10m = windgust_10m[i];
      windgustdir_10m = windgustdirection_10m[i];
    }
  }

  //Total rainfall for the day is calculated within the interrupt
  //Calculate amount of rainfall for the last 60 minutes
  rainin = 0;
  for (int i = 0 ; i < 60 ; i++)
    rainin += rainHour[i];
}

//Returns the instataneous wind speed
float get_wind_speed()
{
  float deltaTime = millis() - lastWindCheck; //750ms

  deltaTime /= 1000.0; //Covert to seconds

  float windSpeed = (float)windClicks / deltaTime; //3 / 0.750s = 4

  windClicks = 0; //Reset and start watching for new wind
  lastWindCheck = millis();

  windSpeed *= 1.492; //4 * 1.492 = 5.968MPH

  /* Serial.println();
    Serial.print("Windspeed:");
    Serial.println(windSpeed);*/

  return (windSpeed);
}

//Read the wind direction sensor, return heading in degrees
int get_wind_direction()
{
  unsigned int adc;

  adc = analogRead(WDIR); // get the current reading from the sensor

  // The following table is ADC readings for the wind direction sensor output, sorted from low to high.
  // Each threshold is the midpoint between adjacent headings. The output is degrees for that ADC reading.
  // Note that these are not in compass degree order! See Weather Meters datasheet for more information.

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
  return (-1); // error, disconnected?
}


//Prints the various variables directly to the port
//I don't like the way this function is written but Arduino doesn't support floats under sprintf
void printWeather()
{
  calcWeather(); //Go calc all the various sensors

  Serial.println();
  Serial.print("$,winddir=");
  Serial.print(winddir);
  Serial.print(",windspeedmph=");
  Serial.print(windspeedmph, 1);
  Serial.print(",windgustmph=");
  Serial.print(windgustmph, 1);
  Serial.print(",windgustdir=");
  Serial.print(windgustdir);
  Serial.print(",windspdmph_avg2m=");
  Serial.print(windspdmph_avg2m, 1);
  Serial.print(",winddir_avg2m=");
  Serial.print(winddir_avg2m);
  Serial.print(",windgustmph_10m=");
  Serial.print(windgustmph_10m, 1);
  Serial.print(",windgustdir_10m=");
  Serial.print(windgustdir_10m);
  Serial.print(",rainin=");
  Serial.print(rainin, 2);
  Serial.print(",dailyrainin=");
  Serial.print(dailyrainin, 2);
  Serial.print(",");
  Serial.println("#");
}
