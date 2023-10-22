#include "Particle.h"
// Temperature, humidity, air pressure and altitude sensor library
#include "Adafruit_BME280.h"
// Library that converts data into JASON Parser format
#include "JsonParserGeneratorRK.h"


Adafruit_BME280 bme;

//initialising variables for BME280 sensor
int temp = 0;
int humidity = 0;
int pressure = 0;
int altitude = 0;


// BAROMETER SENSOR CODE


//Function to collect all barometer sensor readings
void getBarometerReadings() {
  //Collects readings from sensors
  //Assigns temp variable to temperature, collected by sensor
  temp = (int)bme.readTemperature();
  
  //Assigns humidity variable to temperature, collected by sensor
  humidity = (int)bme.readHumidity();
  
  //Assigns pressure variable to temperature, collected by sensor
  pressure = (int)bme.readPressure();
  
  //Assigns altitude variable to temperature, collected by sensor
  altitude = (int)bme.readAltitude(1013.25); //sea level pressure, estimates altitude
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
    //Assigns air quality to dangerous level
    airQuality = "Dangerous Level";
  }
  else if (airQualityVal == AirQualitySensor:: HIGH_POLLUTION) {
    //Assigns air quality to high pollution
    airQuality = "High Pollution";
  }
  else if (airQualityVal == AirQualitySensor:: LOW_POLLUTION) {
    //Assigns air quality to low pollution 
    airQuality = "Low Pollution";
  }
  //Assigns air quality to fresh air
  else if (airQualityVal == AirQualitySensor:: FRESH_AIR) {
    airQuality = "Fresh Air";
  }
  else {
    //A validation check for when the reading for air quality isn't successful
    airQuality = "Reading Unsuccessful";
  }

}

// DUST SENSOR

#include <math.h>
#define DUST_SENSOR_PIN A2
#define SENSOR_READING_INTERVAL 30000


//Variables used in program
//The miliseconds since the last readings check
unsigned long lastCheck;
//Stores the value for lowpulseoccupancy
unsigned long lpo_val = 0;
//Stores the value for the last lowpulseoccupancy
unsigned long last_lpo = 0;
unsigned long duration;

double ratio = 0;
//Concentration of dust in atmosphere
double concentration = 0;


//Function of collect sensor readings
void getDustSensorReadings(){
  //Prevents the value on the dust sensor from being 0
  if (lpo_val == 0){
    lpo_val = last_lpo;
  }
  else{
    last_lpo = lpo_val;
  }

  //Pre-set formulas to calculates the concentration of dust
  ratio = lpo_val / (SENSOR_READING_INTERVAL * 10.0);
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


// MQTT Broker connection
#include "MQTT.h"

//const long channelId = 1883; 
String clientId = "mrargon";
//String username = "lucyargon22";
//String password = "myproject22!";
char server[ ] = "public.mqtthq.com";


//Setup MQTT broker
MQTT client(server, 1883, callback); 

// Define a callback function to initialize the MQTT client.
void callback(char* topic, byte* payload, unsigned int length) {
}

void mqtt_publish(char *pubdata){
  client.publish("weather/station/data",pubdata);
}

void reconnect(){
  Particle.publish("Attempting MQTT connection");
        
  // Connect to the HiveMQ MQTT broker.
//  Serial.print("Client if statement output: ");
  int connected = client.connect(clientId);
  if (!connected)  {
    // Track the connection with particle console.
    Particle.publish("Connected");
  } 
  else {
    String connectionCode = "Connection return code: " + (String)connected;
    Particle.publish(connectionCode);
    Particle.publish("Failed to connect. Trying to reconnect in 2 seconds");
    delay(2000);
  } 
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

  client.connect(clientId);
  Particle.publish((String)client.isConnected());
}


void loop() {

  getBarometerReadings();
  getAirQualityReadings();
  getLightReadings();
  getSoundReadings();

  duration = pulseIn(DUST_SENSOR_PIN, LOW);
  
  lpo_val = lpo_val + duration;

  if ((millis() - lastCheck) > SENSOR_READING_INTERVAL)
  {
    getDustSensorReadings();
  
    lpo_val = 0;
    lastCheck = millis();
  }

  


  //Build JSON object to publish to cloud

  JsonWriterStatic<256> jw;

  {
    JsonWriterAutoObject obj(&jw);

    // Add various types of data
    jw.insertKeyValue("timestamp", (int) Time.now());
    jw.insertKeyValue("temp", temp);
    jw.insertKeyValue("humidity", humidity);
    jw.insertKeyValue("pressure", pressure);
    jw.insertKeyValue("altitude", altitude);
    jw.insertKeyValue("airQual", airQuality);
    jw.insertKeyValue("lpo_val", lpo_val);
    jw.insertKeyValue("dust_ratio", ratio);
    jw.insertKeyValue("dust_conc", concentration);
    jw.insertKeyValue("light", lightVal);
    jw.insertKeyValue("sound", soundVal);

  }
  // If MQTT client is not connected then reconnect.
  if (!client.isConnected()) {
    reconnect();
  } 
  
  
  mqtt_publish(jw.getBuffer());
  Particle.publish("weatherStationData", jw.getBuffer(), PUBLIC);


    
  // Call the loop continuously to establish connection to the server.
  
  if (client.isConnected()) {
    client.loop();
  }
  //Only take readings at certain intervals (in minutes)
  int delay_interval = 20;
  delay(delay_interval*60000);
  
}
