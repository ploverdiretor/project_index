void motor_drv(){
  esp_err_t result;
  unsigned short sensorValue0 = analogRead(2);
  unsigned short sensorValue1 = analogRead(3);
  double batt_voltage = ((sensorValue1 * (3.3 / 4095.0)) / 0.180328)*0.867092; // 0.180328
  batt_value = 12.75 / batt_voltage;
  if (batt_voltage < 9.5){
    digitalWrite(10,HIGH);
  }
  voltage1 = ((sensorValue0>>1) - pid_P2 - pid_R2 + wifi_throttle + wifi_rudder + wifi_vr00 + wifi_vr10) * batt_value; //BLDC ch1 FL
  voltage2 = ((sensorValue0>>1) + pid_P2 - pid_R2 + wifi_throttle - wifi_rudder - wifi_vr00 + wifi_vr10) * batt_value; //BLDC ch2 BL
  voltage3 = ((sensorValue0>>1) - pid_P2 + pid_R2 + wifi_throttle - wifi_rudder + wifi_vr00 - wifi_vr10) * batt_value; //BLDC ch3 FR
  voltage4 = ((sensorValue0>>1) + pid_P2 + pid_R2 + wifi_throttle + wifi_rudder - wifi_vr00 - wifi_vr10) * batt_value; //BLDC ch4 BR
  if (wifi_buttonL == 0){
    m_alarm = 0;
    digitalWrite(10,LOW);
  }
    //------------------------BLDC CH1
  if (voltage1 > 2047){voltage1 = 2047;}
  if ((voltage1 < 0) || (m_alarm == 1)){voltage1 = 0;}
  //------------------------BLDC CH2
  if (voltage2 > 0x2047){voltage2 = 0x2047;}
  if ((voltage2 < 0) || (m_alarm == 1)){voltage2 = 0;}
  //------------------------BLDC CH3
  if (voltage3 > 0x2047){voltage3 = 0x2047;}
  if ((voltage3 < 0) || (m_alarm == 1)){voltage3 = 0;}
  //------------------------BLDC CH4
  if (voltage4 > 0x2047){voltage4 = 0x2047;}
  if ((voltage4 < 0) || (m_alarm == 1)){voltage4 = 0;}
  throttleAverage = (voltage1 + voltage2 + voltage3 + voltage4)/4;
  if (throttleAverage > 768){
    integralOn = 1;
    digitalWrite(4,HIGH);
  } else {
    integralOn = 0;
    digitalWrite(4,LOW);
  }
  //-----
  int lsb1 = voltage1 & 0x000F;
  int msb1 = voltage1 >> 4;
  cmd = 0x80 | (byte)lsb1;
  bldc_drv(cmd,msb1);
  int lsb2 = voltage2 & 0x000F;
  int msb2 = voltage2 >> 4;
  cmd = 0xA0 | (byte)lsb2;
  bldc_drv(cmd,msb2);
  int lsb3 = voltage3 & 0x000F;
  int msb3 = voltage3 >> 4;
  cmd = 0xC0 | (byte)lsb3;
  bldc_drv(cmd,msb3);
  int lsb4 = voltage4 & 0x000F;
  int msb4 = voltage4 >> 4;
  cmd = 0xE0 | (byte)lsb4;
  bldc_drv(cmd,msb4);

  int lsb5 = sensorValue1 & 0x000F;
  int msb5 = sensorValue1 >> 4;
  s_dat[0] = 0x20;
  s_dat[1] = msb5;
  result = esp_now_send(slave.peer_addr, s_dat, sizeof(s_dat));
  s_dat[0] = 0x21;
  s_dat[1] = lsb5;
  result = esp_now_send(slave.peer_addr, s_dat, sizeof(s_dat));
}