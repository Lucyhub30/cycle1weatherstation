/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#line 1 "/Users/lucy/Desktop/AlevelProject/cycle1weatherstation/src/cycle1weatherstation.ino"
#include "Particle.h"
#include "Adafruit_BME280.h"
#include "JsonParserGeneratorRK.h"


void getBarometerReadings();
void getAirQualityReadings();
void getDustSensorReadings();
void getLightReadings();
void getSoundReadings();
void setup();
void loop();
#line 6 "/Users/lucy/Desktop/AlevelProject/cycle1weatherstation/src/cycle1weatherstation.ino"
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
#define AQS_PIN D2
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


unsigned long lastCheck;
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

}


// LIGHT SENSOR
int lightPin = A0;
int lightVal = 0;
void getLightReadings() {

  lightVal = analogRead(lightPin);

}

// SOUND SENSOR
int soundPin = A4;
int soundVal;

void getSoundReadings() {
  soundVal = analogRead(soundPin);
}


void setup() {

  Particle.publish("Weather Station Online :)");

  //Setup barometer sensor
  bme.begin();

  //Setup serial monitor
  Serial.begin(9600);
  
  //Setup dust sensor
  pinMode(DUST_SENSOR_PIN, INPUT);
  lastCheck = millis();

  //Setup light sensor
  pinMode(lightPin, INPUT);

  //Setup sound sensor
  pinMode(soundPin, INPUT);

  
}


void loop() {

  getBarometerReadings();
  getAirQualityReadings();
  getLightReadings();
  getSoundReadings();
  


  duration = pulseIn(DUST_SENSOR_PIN, LOW);
  
  lowpulseoccupancy = lowpulseoccupancy + duration;

  if ((millis() - lastCheck) > SENSOR_READING_INTERVAL)
  {
    getDustSensorReadings();
  
    lowpulseoccupancy = 0;
    lastCheck = millis();
  }

  Serial.print("Temperature: ");
  Serial.println(temp);

  //Build JSON object to publish to cloud
  JsonWriterStatic<256> jw;

  {
    JsonWriterAutoObject obj(&jw);

    // Add various types of data
    jw.insertKeyValue("temp", temp);
    jw.insertKeyValue("humidity", humidity);
    jw.insertKeyValue("pressure", pressure);
    jw.insertKeyValue("altitude", altitude);
    jw.insertKeyValue("airQual", airQuality);
    jw.insertKeyValue("lpo_val", lowpulseoccupancy);
    jw.insertKeyValue("dust_ratio", ratio);
    jw.insertKeyValue("dust_conc", concentration);
    jw.insertKeyValue("light", lightVal);
    jw.insertKeyValue("sound", soundVal);

  }
  Particle.publish("weatherStationData", jw.getBuffer(), PRIVATE);
}
