float readLevel(int trig, int echo)
{
  Serial.println("Read Level");
  prev_distance = distanceCm;
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echo, HIGH);
  Serial.println(duration);
  // Calculate the distance
  distanceCm = duration * SOUND_VELOCITY/2;
  Serial.println(distanceCm);
  return distanceCm;
}
