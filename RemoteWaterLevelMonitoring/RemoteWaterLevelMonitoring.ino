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
#define DEVICE_ID "LevelMon"
#define SOUND_VELOCITY 0.034
#define CM_TO_INCH 0.393701

long duration;
float distanceCm;
float prev_distance;
float thresh = 0.2;
float distanceInch;
const int trigPin = 5;
const int echoPin = 4;
long influxCounter=500;
int ohh,ohm,ohl,glh,glm,gll,suh,summ,sul;
int sumpState;

#define TZ_INFO "Asia/Kolkata"

// InfluxDB client for InfluxDB Cloud API
InfluxDBClient client_cloud(INFLUXDB_CLOUD_URL, INFLUXDB_CLOUD_ORG, INFLUXDB_CLOUD_BUCKET, INFLUXDB_CLOUD_TOKEN, InfluxDbCloud2CACert);


// because we use the deepsleep pattern we put all our logic in the setup() routine and then send the microcontroller back to sleep
//  if your microcontroller is always up you might want to put the commands in your loop() routine instead
void readlevel()
{
  Serial.println("Read Level");
  prev_distance = distanceCm;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  Serial.println(duration);
  // Calculate the distance
  distanceCm = duration * SOUND_VELOCITY/2;
  Serial.println(distanceCm);
}
void sendInflux()
{
  float sensor_value1 = distanceCm;
  float sensor_value2 = 74-distanceCm;
  // END: read sensor values
  Point pointDevice("mymeasurement"); // create a new measurement point (the same point can be used for Cloud and v1 InfluxDB)
  // add tags to the datapoints so you can filter them
  pointDevice.addTag("device", DEVICE_ID);
  pointDevice.addTag("SSID", WiFi.SSID());
  // Add data fields (values)
  pointDevice.addField("sensorThanal1", sensor_value1);
  pointDevice.addField("sensorThanal2", sensor_value2);
  pointDevice.addField("uptime", millis()); // in addition send the uptime of the Arduino
  pointDevice.addField("Strength",WiFi.RSSI());
   timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");

  // Check server connection
  if (client_cloud.validateConnection()) {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(client_cloud.getServerUrl());
  } else {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(client_cloud.getLastErrorMessage());
  }
  Serial.println("writing to InfluxDB Cloud... ");
  if (!client_cloud.writePoint(pointDevice)) {
    Serial.print("InfluxDB write failed: ");
    Serial.println(client_cloud.getLastErrorMessage());
  }
  else
  {
    Serial.println("Success");
  }

}
void sendRawInflux()
{
  Point pointDevice("myRawmeasurement"); // create a new measurement point (the same point can be used for Cloud and v1 InfluxDB)
  pointDevice.addTag("device", DEVICE_ID);
  pointDevice.addTag("SSID", WiFi.SSID());
  pointDevice.addField("overheadHigh", ohh);
  pointDevice.addField("overheadMed", ohm);
  pointDevice.addField("overheadLow", ohl);
  pointDevice.addField("groundHigh", glh);
  pointDevice.addField("groundMed", glm);
  pointDevice.addField("groundLow", gll);
  pointDevice.addField("sumpHigh", suh);
  pointDevice.addField("sumpMed", summ);
  pointDevice.addField("sumpLow", sul);
  pointDevice.addField("uptime", millis()); // in addition send the uptime of the Arduino
  pointDevice.addField("Strength",WiFi.RSSI());
  timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");
  // Check server connection
  if (client_cloud.validateConnection()) {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(client_cloud.getServerUrl());
  } else {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(client_cloud.getLastErrorMessage());
  }
  Serial.println("writing to InfluxDB Cloud... ");
  if (!client_cloud.writePoint(pointDevice)) {
    Serial.print("InfluxDB write failed: ");
    Serial.println(client_cloud.getLastErrorMessage());
  }
  else
  {
    Serial.println("Success");
  }
}
int readInflux()
{
  String query = "from(bucket: \"thanal1\'s Bucket\")|> range(start: -48h, stop: -1m)|> filter(fn: (r) => r[\"_measurement\"] == \"mymeasurement\")|> filter(fn: (r) => r[\"button\"] == \"sumpbutton\")|> filter(fn: (r) => r[\"_field\"] == \"bval\")|> yield(name: \"last\")";
  double sumpValue=-1;
    FluxQueryResult result = client_cloud.query(query);
      while (result.next()) {
        sumpValue = result.getValueByName("_value").getDouble();
      }
    return sumpValue;//send the last available value

}
void setup() {
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  Serial.begin(115200);
  Serial.println("Starting setup");
  delay(100);  
  wifiConnect();
  // BEGIN: read sensor values (add your real code here)
  // readlevel();
  // sendInflux();  
  // this sends the microcontroller to deepsleep until the next reading needs to be taken
  // due to WiFi connect and sensor reading your measurement interval will always be 5~10 seconds longer than the SLEEP_S duration
  // ESP.deepSleep(SLEEP_S * 1000000); // offset by the duration the program run (converted from ms to µs)
}

void loop() {
  delay(10);
  if(influxCounter>=3700)
  {
    influxCounter=0;
    readlevel();
    sendInflux();
    Serial.println("Will send data to influx in 37 sec...");
    //also check the status of solenoid
    double readVal = readInflux();
    if(readVal!=-1)
    {
      if(readVal==0)
        Serial.println('^');
       if(readVal==1)
        Serial.println('&');
    }
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
      sendRawInflux();
    }
  }
  influxCounter++;
}

void processandStoreRawValue(String recv)
{
  if((recv.indexOf(">")==0)&&(recv.indexOf("<")>=5))
  {
    recv.replace(">","");
    recv.replace("<","");
    ohh = getValue(recv,'|',0);
    ohm = getValue(recv,'|',1);
    ohl = getValue(recv,'|',2);
    glh = getValue(recv,'|',3);
    glm = getValue(recv,'|',4);
    gll = getValue(recv,'|',5);
    suh = getValue(recv,'|',6);
    summ = getValue(recv,'|',7);
    sul = getValue(recv,'|',8);
    sumpState = getValue(recv,'|',9);
  }
  Serial.print(recv);
}
int getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  String iString = found>index ? data.substring(strIndex[0], strIndex[1]) : "";
  return iString.toInt();
}
// try to connect to given SSID and key, loop until successful
void wifiConnect() {
   Serial.println("Scanning Wifi Networks...");
   int n = WiFi.scanNetworks();
   Serial.print(n);Serial.println("Wifi networks found. Trying to connect to 2 known networks");
   for (int i = 0; i < n; ++i) {
     Serial.println(WiFi.SSID(i));
     if (WiFi.SSID(i)== WIFI_SSID1) {
        WiFi.begin(WIFI_SSID1,WIFI_KEY1);
        break;
     }
     if (WiFi.SSID(i)== WIFI_SSID2) {
       WiFi.begin(WIFI_SSID2,WIFI_KEY2);
       break;
     }
    }
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Successfully connected to ");
  Serial.println(WiFi.SSID());
}
