
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
