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

void setup() {
  bme.begin();

  Serial.begin(9600);

  //set variables of particle dashboard
  Particle.variable("temp", temp);
  Particle.variable("humidity", humidity);
  Particle.variable("pressure", pressure);
  Particle.variable("altitude", altitude);

  Particle.publish("Weather Station Online :)");
}


void loop() {

  getBarometerReadings();

  Particle.publish("testing");

  Serial.print("Temperature: ");
  Serial.println(temp);

  Serial.print("Humidity: ");
  Serial.println(humidity);

  Serial.print("Pressure: ");
  Serial.println(pressure);

  Serial.print("Altitude: ");
  Serial.println(altitude);

}