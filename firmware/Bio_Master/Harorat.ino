void harorat () {
  if (dhtTemperature <= T1) {
  digitalWrite(rele2, HIGH);
  digitalWrite(rele3, HIGH);
  }
  else if (dhtTemperature >= T2) {
  digitalWrite(rele3, HIGH);
  }
  else {
    digitalWrite(rele2, LOW);
    digitalWrite(rele3, LOW);
  }
}
