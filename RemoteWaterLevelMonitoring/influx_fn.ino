// InfluxDB client for InfluxDB Cloud API
InfluxDBClient client_cloud(INFLUXDB_CLOUD_URL, INFLUXDB_CLOUD_ORG, INFLUXDB_CLOUD_BUCKET, INFLUXDB_CLOUD_TOKEN, InfluxDbCloud2CACert);


void sendRawInflux(bool hasADCVal,float oh_lev, float gnd_lev)
{
  Point pointDevice("thanal_measurement"); // create a new measurement point (the same point can be used for Cloud and v1 InfluxDB)
  pointDevice.addTag("device", DEVICE_ID);
  pointDevice.addTag("SSID", WiFi.SSID());
  if(hasADCVal)
  {
    //pointDevice.addField("overheadHigh", ohh);
    //pointDevice.addField("overheadMed", ohm);
    //pointDevice.addField("overheadLow", ohl);
    //pointDevice.addField("groundHigh", glh);
    //pointDevice.addField("groundMed", glm);
    //pointDevice.addField("groundLow", gll);
    pointDevice.addField("sumpHigh", suh);
    pointDevice.addField("sumpMed", summ);
    pointDevice.addField("sumpLow", sul);
  }
  pointDevice.addField("overhead_dist",oh_lev);
  pointDevice.addField("ground_dist",gnd_lev);
  pointDevice.addField("uptime", millis()); // in addition send the uptime of the Arduino
  pointDevice.addField("Strength",WiFi.RSSI());
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
