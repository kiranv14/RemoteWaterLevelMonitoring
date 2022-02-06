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
