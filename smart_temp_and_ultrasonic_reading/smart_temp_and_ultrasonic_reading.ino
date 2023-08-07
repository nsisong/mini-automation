#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <WiFiManager.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define bulb              D2
#define Socket_pin        D1
#define echo_pin          D5
#define trig_pin          D8




/************************* Dallas temp Setup *********************************/

// GPIO where the DS18B20 is connected to
const int oneWireBus = D7;     

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);


/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com" //Adafruit Server
#define AIO_SERVERPORT  1883                   
#define AIO_USERNAME    "mkut22"            // Username
#define AIO_KEY         "aio_TvPX83aL7pHNa35CPuKhfFK0YA07"   // Auth Key

//WIFI CLIENT
WiFiClient client;

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

Adafruit_MQTT_Subscribe Light = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME"/feeds/bulb"); // Feeds name should be same everywhere
Adafruit_MQTT_Subscribe socket = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME"/feeds/socket"); // Feeds name should be same everywhere
Adafruit_MQTT_Publish temp = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME"/feeds/Temp_feed"); // Feeds name should be same everywhere
Adafruit_MQTT_Publish ultra = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME"/feeds/ultrasonic"); // Feeds name should be same everywhere


void MQTT_connect();
float ultrasonic_sens();
float temp_sens();

void setup() {
  Serial.begin(115200);

  sensors.begin();

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  //sets timeout until configuration portal gets turned off
  //useful to make it all retry or go to sleep
  //in seconds

  wifiManager.setConfigPortalTimeout(180);
  
  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if(!wifiManager.autoConnect("MkuT")) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.restart();
    delay(5000);
    } 

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");

  pinMode(bulb,  OUTPUT);
  pinMode(Socket_pin,  OUTPUT);
  pinMode(echo_pin, INPUT);
  pinMode(trig_pin, OUTPUT);

 
  mqtt.subscribe(&Light);
  mqtt.subscribe(&socket);

}



void loop() {

  MQTT_connect();
  

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(20000))) {
    if (subscription == &Light) {
      Serial.print(F("Got: "));
      Serial.println((char *)Light.lastread);
      int Light_State = atoi((char *)Light.lastread);
      digitalWrite(bulb, Light_State);
    }

    if (subscription == &socket) {
      Serial.print(F("Got: "));
      Serial.println((char *)socket.lastread);
      int socket_State = atoi((char *)socket.lastread);
      digitalWrite(Socket_pin, socket_State);
    }

  }

  temp.publish(temp_sens());
  ultra.publish(ultrasonic_sens());

}


void MQTT_connect() {
  int8_t ret;

  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  
  while ((ret = mqtt.connect()) != 0) {
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000); 
    retries--;
    if (retries == 0) {
      while (1);
    }
  }
  Serial.println("MQTT Connected!");
  
}


float ultrasonic_sens(){
  digitalWrite(trig_pin, LOW);
  delayMicroseconds(2);
  
  digitalWrite(trig_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig_pin, LOW);
  
  unsigned long pulseDuration = pulseIn(echo_pin, HIGH);
  
  float distance = pulseDuration * 0.034 / 2;
  return distance;

}
float temp_sens(){
  sensors.requestTemperatures(); 
  float temperatureC = sensors.getTempCByIndex(0);
  delay(1000);
  return temperatureC;
}