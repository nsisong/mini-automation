#define BLYNK_TEMPLATE_ID "TMPL2ssbFj6-p"
#define BLYNK_TEMPLATE_NAME "mkutiot"
#define BLYNK_AUTH_TOKEN "-rf9rC2ylI5cJ_L0hfJd6xAMnvIlrNeY"

// #define BLYNK_PRINT Serial            
#include <BlynkSimpleEsp8266.h> 
#include <WiFiManager.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>

// define the GPIO connected with Relays and switches
#define bulb              D2
#define Socket_pin        D1
#define echo_pin          D5
#define trig_pin          D8

#define Socket         V1 
#define Temperature    V2
#define Ultrasonic     V3 
#define Bulb           V0

int toggleState_4 = LOW;
int toggleState_1 = LOW;


int wifiFlag = 0;

/************************* Dallas temp Setup *********************************/

// GPIO where the DS18B20 is connected to
const int oneWireBus = D7;     

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);


BlynkTimer timer;

//WIFI CLIENT
WiFiClient client;

void ultrasonic_sens(){
  digitalWrite(trig_pin, LOW);
  delayMicroseconds(2);
  
  digitalWrite(trig_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig_pin, LOW);
  
  unsigned long pulseDuration = pulseIn(echo_pin, HIGH);
  
  float distance = pulseDuration * 0.034 / 2;
  Blynk.virtualWrite(Ultrasonic, distance);
  // return distance;

}
void temp_sens(){
  sensors.requestTemperatures(); 
  float temperatureC = sensors.getTempCByIndex(0);
  delay(1000);
  Blynk.virtualWrite(Temperature, temperatureC);
  // return temperatureC;
}


BLYNK_CONNECTED() {
  // Request the latest state from the server
  Blynk.syncVirtual(Socket);
  Blynk.syncVirtual(Bulb);
  // Blynk.virtualWrite();
  // Blynk.virtualWrite();
}

// When App button is pushed - switch the state

BLYNK_WRITE(Socket) {
  toggleState_1 = param.asInt();
  digitalWrite(Socket_pin, toggleState_1);
}

BLYNK_WRITE(Temperature) {
  // toggleState_2 = param.asInt();
  // digitalWrite(RelayPin2, toggleState_2);
}

BLYNK_WRITE(Ultrasonic) {
  // toggleState_3 = param.asInt();
  // digitalWrite(RelayPin3, toggleState_3);
}

BLYNK_WRITE(Bulb) {
  toggleState_4 = param.asInt();
  Serial.print("toggleState_4: ");
  Serial.println(toggleState_4 );
  digitalWrite(bulb, toggleState_4);
}


void checkBlynkStatus() { // called every 3 seconds by SimpleTimer

  bool isconnected = Blynk.connected();
  if (isconnected == false) {
    wifiFlag = 1;
    // Serial.println("Not connected to blynk");
  }
  if (isconnected == true) {
    wifiFlag = 0;
    // Serial.println("Connected to blynk");
  }
}


void setup()
{
  Serial.begin(9600);
  sensors.begin();

  // WiFiManager
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

  pinMode(bulb,        OUTPUT);
  pinMode(Socket_pin,  OUTPUT);
  pinMode(echo_pin,    INPUT);
  pinMode(trig_pin,    OUTPUT);


  // timer.setInterval(3000L, checkBlynkStatus); // check if Blynk server is connected every 3 seconds
  timer.setInterval(100L, ultrasonic_sens);
  timer.setInterval(100L, temp_sens);
  Blynk.config(BLYNK_AUTH_TOKEN);
}

void loop()
{  

  Blynk.run();


  timer.run(); // Initiates SimpleTimer

}
