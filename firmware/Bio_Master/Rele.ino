void datarele () {
  digitalWrite(rele, val3);
  if (val3 == 1) {
    val2 = true;
  }
  else {
    val2 = false;
  }  
}

void dataphoto () {
  digitalWrite(rele1, val3);
  if (val3 == 1) {
    val4 = true;
    dataldr = 1;
  }
  else {
    val4 = false;
    dataldr = 0;
  }  
}
