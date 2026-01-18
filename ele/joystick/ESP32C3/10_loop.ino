void loop() {
  esp_err_t result;
  //---------------------------------------------------------------------半固定00
  get_adValue1(0);
  unsigned int dataCh00 = ((ad_HighByte & 0x0f) << 8) + ad_LowByte;
  int volts00 = (int)(dataCh00 >>5);
  s_dat[0] = 0x10;
  s_dat[1] = volts00;
  result = esp_now_send(slave.peer_addr, s_dat, sizeof(s_dat));
  //---------------------------------------------------------------------半固定01
  get_adValue1(1);
  unsigned int dataCh01 = ((ad_HighByte & 0x0f) << 8) + ad_LowByte;
  int volts01 = (int)(dataCh01 >>5);
  s_dat[0] = 0x11;
  s_dat[1] = volts01;
  result = esp_now_send(slave.peer_addr, s_dat, sizeof(s_dat));
  //---------------------------------------------------------------------半固定02
  get_adValue1(2);
  unsigned int dataCh02 = ((ad_HighByte & 0x0f) << 8) + ad_LowByte;
  int volts02 = (int)(dataCh02 >>5);
  s_dat[0] = 0x12;
  s_dat[1] = volts02;
  result = esp_now_send(slave.peer_addr, s_dat, sizeof(s_dat));
  //---------------------------------------------------------------------半固定03
  get_adValue1(3);
  unsigned int dataCh03 = ((ad_HighByte & 0x0f) << 8) + ad_LowByte;
  int volts03 = (int)(dataCh03 >>5);
  s_dat[0] = 0x13;
  s_dat[1] = volts03;
  result = esp_now_send(slave.peer_addr, s_dat, sizeof(s_dat));
  //---------------------------------------------------------------------半固定10
  get_adValue1(4);
  unsigned int dataCh04 = ((ad_HighByte & 0x0f) << 8) + ad_LowByte;
  int volts04 = (int)(dataCh04 >>5);
  s_dat[0] = 0x14;
  s_dat[1] = volts04;
  result = esp_now_send(slave.peer_addr, s_dat, sizeof(s_dat));
  //---------------------------------------------------------------------半固定11
  get_adValue1(5);
  unsigned int dataCh05 = ((ad_HighByte & 0x0f) << 8) + ad_LowByte;
  int volts05 = (int)(dataCh05 >>5);
  s_dat[0] = 0x15;
  s_dat[1] = volts05;
  result = esp_now_send(slave.peer_addr, s_dat, sizeof(s_dat));
  //---------------------------------------------------------------------半固定12
  get_adValue1(6);
  unsigned int dataCh06 = ((ad_HighByte & 0x0f) << 8) + ad_LowByte;
  int volts06 = (int)(dataCh06 >>5);
  s_dat[0] = 0x16;
  s_dat[1] = volts06;
  result = esp_now_send(slave.peer_addr, s_dat, sizeof(s_dat));
  //---------------------------------------------------------------------半固定13
  get_adValue1(7);
  unsigned int dataCh07 = ((ad_HighByte & 0x0f) << 8) + ad_LowByte;
  int volts07 = (int)(dataCh07 >>5);
  s_dat[0] = 0x17;
  s_dat[1] = volts07;
  result = esp_now_send(slave.peer_addr, s_dat, sizeof(s_dat));
  //---------------------------------------------------------------------スロットル
  get_adValue2(0);
  unsigned int dataCh10 = ((ad_HighByte & 0x0f) << 8) + ad_LowByte;
  int volts10 = ad_table[255-(int)(dataCh10 >>4)]/2;
  s_dat[0] = 0x01;
  s_dat[1] = volts10;
  result = esp_now_send(slave.peer_addr, s_dat, sizeof(s_dat));
  //---------------------------------------------------------------------ラダー
  get_adValue2(1);
  unsigned int dataCh11 = ((ad_HighByte & 0x0f) << 8) + ad_LowByte;
  int volts11 = ad_table[255-(int)(dataCh11 >>4)]/2;
  s_dat[0] = 0x02;
  s_dat[1] = volts11;
  result = esp_now_send(slave.peer_addr, s_dat, sizeof(s_dat));
  //---------------------------------------------------------------------ピッチ
  get_adValue2(2);
  unsigned int dataCh12 = ((ad_HighByte & 0x0f) << 8) + ad_LowByte;
  int volts12 = ad_table[255-(int)(dataCh12 >>4)]/2;
  s_dat[0] = 0x03;
  s_dat[1] = volts12;
  result = esp_now_send(slave.peer_addr, s_dat, sizeof(s_dat));
  //---------------------------------------------------------------------ロール
  get_adValue2(3);
  unsigned int dataCh13 = ((ad_HighByte & 0x0f) << 8) + ad_LowByte;
  int volts13 = ad_table[255-(int)(dataCh13 >>4)]/2;
  s_dat[0] = 0x04;
  s_dat[1] = volts13;
  result = esp_now_send(slave.peer_addr, s_dat, sizeof(s_dat));
  //---------------------------------------------------------------------SW-L
  get_adValue2(4);
  unsigned int dataCh14 = ((ad_HighByte & 0x0f) << 8) + ad_LowByte;
  int volts14 = (int)(dataCh14 >>5);
  s_dat[0] = 0x05;
  if (volts14 > 64){
    s_dat[1] = 1;
  } else {
    s_dat[1] = 0;
  }
  result = esp_now_send(slave.peer_addr, s_dat, sizeof(s_dat));
  //---------------------------------------------------------------------SW-R
  get_adValue2(5);
  unsigned int dataCh15 = ((ad_HighByte & 0x0f) << 8) + ad_LowByte;
  int volts15 = (int)(dataCh15 >>5);
  s_dat[0] = 0x06;
  if (volts15 > 64){
    s_dat[1] = 1;
  } else {
    s_dat[1] = 0;
  }
  result = esp_now_send(slave.peer_addr, s_dat, sizeof(s_dat));
  //---------------------------------------------------------------------DIP-1
  get_adValue2(6);
  unsigned int dataCh16 = ((ad_HighByte & 0x0f) << 8) + ad_LowByte;
  int volts16 = (int)(dataCh16 >>5);
  s_dat[0] = 0x07;
  if (volts16 > 64){
    s_dat[1] = 1;
  } else {
    s_dat[1] = 0;
  }
  result = esp_now_send(slave.peer_addr, s_dat, sizeof(s_dat));
  //---------------------------------------------------------------------DIP-2
  get_adValue2(7);
  unsigned int dataCh17 = ((ad_HighByte & 0x0f) << 8) + ad_LowByte;
  int volts17 = (int)(dataCh17 >>5);
  s_dat[0] = 0x08;
  if (volts17 > 64){
    s_dat[1] = 1;
  } else {
    s_dat[1] = 0;
  }
  result = esp_now_send(slave.peer_addr, s_dat, sizeof(s_dat));
  //---------------------------------------------------------------------ActiveSense
  s_dat[0] = 0x00;
  s_dat[1] = 1;
  result = esp_now_send(slave.peer_addr, s_dat, sizeof(s_dat));
  //============================================================================
  lcd.setCursor(0,0);
  lcd.print(strPad(vr_table[volts00],3));
  lcd.setCursor(4,0);
  lcd.print(strPad(volts01,3));
  lcd.setCursor(9,0);
  lcd.print(strPad(volts02,3));
  lcd.setCursor(13,0);
  lcd.print(strPad(volts03,3));
  lcd.setCursor(0,1);
  lcd.print(strPad(vr_table[volts04],3));
  lcd.setCursor(4,1);
  lcd.print(strPad(volts05,3));
  lcd.setCursor(9,1);
  lcd.print(strPad(volts06,3));
  lcd.setCursor(13,1);
  lcd.print(strPad(volts07,3));

  battTotal = battTotal - battVal[battIndex];
  battVal[battIndex] = batt_voltage;
  battTotal = battTotal + battVal[battIndex];
  if (battIndex >= 9){
    battIndex = 0;
  } else {
    battIndex += 1;
  }
//  battIndex = (battIndex + 10) % battNumReadings;
  float batt_ave = battTotal / battNumReadings;
  batt_value = 12.75 / batt_voltage;
  Serial.print(" Throttle:");
  Serial.print(volts10);
  Serial.print(" Rudder:");
  Serial.print(volts11);
  Serial.print(" Pitch:");
  Serial.print(volts12);
  Serial.print(" Roll:");
  Serial.print(volts13);
  Serial.print(" Lch-SW:");
  Serial.print(volts14);
  Serial.print(" Rch-SW:");
  Serial.print(volts15);
  Serial.print(" Dip01:");
  Serial.print(volts16);
  Serial.print(" Dip02:");
  Serial.print(volts17);
  Serial.print(" Batt_Ave:");
  Serial.print(batt_ave);
  Serial.print(" Batt_value:");
  Serial.print(batt_value);
  Serial.print("\n");
  delay(10);
}