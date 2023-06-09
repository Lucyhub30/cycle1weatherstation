/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#line 1 "/Users/lucy/Desktop/AlevelProject/cycle1weatherstation/src/cycle1weatherstation.ino"
#include "Particle.h"
#include "Adafruit_BME280.h"

void getBarometerReadings();
void getAirQualityReadings();
void getDustSensorReadings();
void setup();
void loop();
#line 4 "/Users/lucy/Desktop/AlevelProject/cycle1weatherstation/src/cycle1weatherstation.ino"
Adafruit_BME280 bme;

//initialising variables for BME280 sensor
int temp = 0;
int humidity = 0;
int pressure = 0;
int altitude = 0;


// BAROMETER SENSOR CODE

void getBarometerReadings() {
  //Collects readings from sensors
  temp = (int)bme.readTemperature();
  humidity = (int)bme.readHumidity();
  pressure = (int)bme.readPressure();
  altitude = (int)bme.readAltitude(1013.25);
}


// AIR QUALITY SENSOR CODE

//Library for air quality sensor
#include "Air_Quality_Sensor.h"

//Pin for air quality sensor
#define AQS_PIN A2
AirQualitySensor aqSensor(AQS_PIN);

//Initiates variable airQuality
String airQuality = "Loading";

//Function that collects readings from AQ sensor

void getAirQualityReadings(){

  //Gets sensor reading
  int airQualityVal = aqSensor.slope();

  //Assigns air quality
  if (airQualityVal == AirQualitySensor:: FORCE_SIGNAL) {
    airQuality = "Dangerous Level";
  }
  else if (airQualityVal == AirQualitySensor:: HIGH_POLLUTION) {
    airQuality = "High Polution";
  }
  else if (airQualityVal == AirQualitySensor:: LOW_POLLUTION) {
    airQuality = "Low Polution";
  }
  else if (airQualityVal == AirQualitySensor:: FRESH_AIR) {
    airQuality = "Fresh Air";
  }
  else {
    airQuality = "Reading Unsuccessful";
  }

}

// DUST SENSOR

#include <math.h>
#define DUST_SENSOR_PIN A2
#define SENSOR_READING_INTERVAL 30000


unsigned long lastInterval;
unsigned long lowpulseoccupancy = 0;
unsigned long last_lpo = 0;
unsigned long duration;

double ratio = 0;
double concentration = 0;



void getDustSensorReadings(){
  if (lowpulseoccupancy == 0){
    lowpulseoccupancy = last_lpo;
  }
  else{
    last_lpo = lowpulseoccupancy;
  }

  ratio = lowpulseoccupancy / (SENSOR_READING_INTERVAL * 10.0);
  concentration = 1.1 * pow(ratio, 3) - 3.8 * pow(ratio, 2) + 520 * ratio + 0.62;



  Serial.printlnf("LPO: %lu", lowpulseoccupancy);
  Serial.printlnf("Ratio: %f%%", ratio);
  Serial.printlnf("Concentration: %f pcs/L", concentration);
}

void setup() {
  bme.begin();

  Serial.begin(9600);
 
  //set variables of particle dashboard
  Particle.variable("temp", temp);
  Particle.variable("humidity", humidity);
  Particle.variable("pressure", pressure);
  Particle.variable("altitude", altitude);
  Particle.variable("airQuality", airQuality);
  Particle.variable("lpo", lowpulseoccupancy);
  Particle.variable("ratio", ratio);
  Particle.variable("conc", concentration);

  Particle.publish("Weather Station Online :)");
  pinMode(DUST_SENSOR_PIN, INPUT);
  lastInterval = millis();
}


void loop() {

  getBarometerReadings();
  getAirQualityReadings();
  


  duration = pulseIn(DUST_SENSOR_PIN, LOW);
  Serial.print("Duration: ");
  Serial.print(duration);
  lowpulseoccupancy = lowpulseoccupancy + duration;

  if ((millis() - lastInterval) > SENSOR_READING_INTERVAL)
  {
    getDustSensorReadings();

    lowpulseoccupancy = 0;
    lastInterval = millis();
  }

  Particle.publish("Altitude: ", String(altitude));
  Particle.publish("Temperature: ", String(temp));
  Particle.publish("Humidity: ", String(humidity));
  Particle.publish("Pressure: ", String(pressure));
  Particle.publish("Air Quality: ", String(airQuality));
  //Particle.publish("Dust concentration: ", String(concentration));
  
  Serial.print("Temperature: ");
  Serial.println(temp);

  Serial.print("Humidity: ");
  Serial.println(humidity);

  Serial.print("Pressure: ");
  Serial.println(pressure);

  Serial.print("Altitude: ");
  Serial.println(altitude);

  Serial.print("Air Quality: ");
  Serial.println(airQuality);

  Serial.print("Dust concentration: ");
  Serial.println(concentration);

}