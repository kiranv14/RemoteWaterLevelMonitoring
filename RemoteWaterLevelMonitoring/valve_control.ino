//to control the valve based on input from InfluxDB
void valveControl()
{
     float readVal = readInflux();
     if(readVal!=-1)
    {
      if(readVal==0)
        Serial.println('^');
       if(readVal==1)
        Serial.println('&');
    }
}
