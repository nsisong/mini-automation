#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <WiFiManager.h>

#define bulb              2
#define Socket_pin        0
#define echo_pin          14
#define trig_pin          12

#define temp_pin          2


/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com" //Adafruit Server
#define AIO_SERVERPORT  1883                   
#define AIO_USERNAME    "mkut22"            // Username
#define AIO_KEY         "aio_AHJl987nxr6gTAAkpdH6EclPV2Qj"   // Auth Key

//WIFI CLIENT
WiFiClient client;

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

Adafruit_MQTT_Subscribe Light = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME"/feeds/bulb"); // Feeds name should be same everywhere
Adafruit_MQTT_Subscribe socket = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME"/feeds/socket"); // Feeds name should be same everywhere
Adafruit_MQTT_Publish temp = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME"/feeds/Temp_feed"); // Feeds name should be same everywhere
Adafruit_MQTT_Publish ultra = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME"/feeds/ultrasonic"); // Feeds name should be same everywhere


void MQTT_connect();
void ultrasonic_sens();
void temp_sens();

void setup() {
  Serial.begin(115200);

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
  pinMode(Socket,  OUTPUT);

 
  mqtt.subscribe(&Light);
  mqtt.subscribe(&socket);
  mqtt.publish(&temp);
  mqtt.publish(&ultra);
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

  }
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


void ultrasonic_sens(){

}
void temp_sens(){

}