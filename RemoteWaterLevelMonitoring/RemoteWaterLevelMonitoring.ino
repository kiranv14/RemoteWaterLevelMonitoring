#if defined(ESP32)
  #include <WiFiMulti.h>
  WiFiMulti wifiMulti;
  #define DEVICE "ESP32"
#elif defined(ESP8266)
  #include <ESP8266WiFiMulti.h>
  ESP8266WiFiMulti wifiMulti;
  #define DEVICE "ESP8266"
#endif
#include <InfluxDbClient.h> // load the client library
#include <InfluxDbCloud.h> // only for InfluxDB Cloud: load SSL certificate and additional method call
#include "secrets.h" // load connection credentials

#define SLEEP_S 30 // how many seconds to sleep between readings
#define DEVICE_ID "thanal_lev"
#define SOUND_VELOCITY 0.034
#define CM_TO_INCH 0.393701

long duration;
float distanceCm;
float prev_distance;
float thresh = 0.2;
float distanceInch;
const int trigPin_oh = 5;
const int echoPin_oh = 4;
const int trigPin_gnd = 14;
const int echoPin_gnd = 12;
long influxCounter_readValve=500;
long influxCounter_getLev=500;
int ohh,ohm,ohl,glh,glm,gll,suh,summ,sul;
int sumpState;
bool getLevelInfo=true;
#define TZ_INFO "Asia/Kolkata"


void setup() {
  pinMode(trigPin_oh, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin_oh, INPUT); // Sets the echoPin as an Input
  pinMode(trigPin_gnd, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin_gnd, INPUT); // Sets the echoPin as an Input
  Serial.begin(115200);
  Serial.println("Starting setup");
  delay(100);  
  wifiConnect();
  timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");
}

void loop() {
  delay(10);
  //check if timeSync is required
  if(millis()>86400000) //restart esp every 24 hours
  {
     ESP.restart(); 
  }
  if(influxCounter_readValve>=3700)//trigger the influx transactions every 37 seconds
  {
    influxCounter_readValve=0;
    valveControl();//check solenoid status
  }
  if(influxCounter_getLev>=3000)//read the tank level every 30 seconds in case it did not happen due to arduino failure
  {
    influxCounter_getLev=0;
    float oh_lev = readLevel(trigPin_oh,echoPin_oh);
    float gnd_lev = readLevel(trigPin_gnd,echoPin_gnd);
    sendRawInflux(true,oh_lev,gnd_lev);
  }
  String recvData = "";
  bool serialRecv=false;
  while(Serial.available())
  {
     char inChar = Serial.read();
     recvData += inChar;
     serialRecv=true;
  }
  if(serialRecv)
  {
    if(recvData.length()>5)
    {
      processandStoreRawValue(recvData);
      Serial.println(recvData);
      influxCounter_getLev=0;
      float oh_lev = readLevel(trigPin_oh,echoPin_oh);
      float gnd_lev = readLevel(trigPin_gnd,echoPin_gnd);
      sendRawInflux(true,oh_lev,gnd_lev);
    }
  }
  influxCounter_getLev++;
  influxCounter_readValve++;
}
