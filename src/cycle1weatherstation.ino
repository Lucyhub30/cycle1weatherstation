#include "Adafruit_BME280.h"

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




void setup() {
  bme.begin();

  Serial.begin(9600);
 
  //set variables of particle dashboard
  Particle.variable("temp", temp);
  Particle.variable("humidity", humidity);
  Particle.variable("pressure", pressure);
  Particle.variable("altitude", altitude);
  Particle.variable("airQuality", airQuality);

  Particle.publish("Weather Station Online :)");
}


void loop() {

  getBarometerReadings();
  getAirQualityReadings();

  Particle.publish("Weather Station Online :)");

  Serial.print("Temperature: ");
  Serial.println(temp);

  Serial.print("Humidity: ");
  Serial.println(humidity);

  Serial.print("Pressure: ");
  Serial.println(pressure);

  Serial.print("Altitude: ");
  Serial.println(altitude);

}