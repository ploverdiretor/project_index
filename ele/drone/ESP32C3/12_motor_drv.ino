void motor_drv(){
  esp_err_t result;
  unsigned short sensorValue0 = analogRead(2);
  
  /* --------------------------- BATT電圧に合わせて出力を調整する -------------------------------*/
  unsigned short sensorValue1 = analogRead(3);
  batt_voltage = ((sensorValue1 * (3.3 / 4095.0)) / 0.180328)*0.867092;
  if (batt_voltage < 1){
    batt_voltage = 12.75;
  }
  batt_value = 12.75 / batt_voltage;
  
  /* --------------------------- BATT電圧<9.5Vで赤色LEDを点灯させる -------------------------------*/
  if (batt_voltage < 9.5){
    digitalWrite(10,HIGH);
  }
  
  /* --------------------------- モーター出力電圧を計算する -------------------------------*/
  voltage1 = ((sensorValue0>>1) - pid_P2 - pid_R2 + pid_Y2 + wifi_throttle - wifi_vr00 +31 + wifi_vr10 ) * batt_value; //BLDC ch1 FL
  voltage2 = ((sensorValue0>>1) + pid_P2 - pid_R2 - pid_Y2 + wifi_throttle + wifi_vr00 -31 + wifi_vr10 ) * batt_value; //BLDC ch2 BL
  voltage3 = ((sensorValue0>>1) - pid_P2 + pid_R2 - pid_Y2 + wifi_throttle - wifi_vr00 +31 - wifi_vr10 ) * batt_value; //BLDC ch3 FR
  voltage4 = ((sensorValue0>>1) + pid_P2 + pid_R2 + pid_Y2 + wifi_throttle + wifi_vr00 -31 - wifi_vr10 ) * batt_value; //BLDC ch4 BR
  
  /* --------------------------- 非常停止ボタンの読み取り -------------------------------*/
  key_on = key_on << 1;
  if ((wifi_buttonL == 0)||(wifi_buttonR == 0)){
    key_on = key_on + 1;
  } else {
    active_key = 0;
  }
  key_on &= B00000111;
  if ((key_on == B00000111)&&(active_key == 0)){
    if(m_alarm == 1){                                   //アラームリセット
      m_alarm = 0;
      digitalWrite(10,LOW);
    }else{                                              //アラームセット
      m_alarm = 1;
      digitalWrite(10,HIGH);
    }
    active_key = 1;
  }

  /*------------------------ モーター電圧のクリップ処理とアラーム制御 ----------------------------*/
  if (voltage1 > 2047){voltage1 = 2047;}
  if ((voltage1 < 0) || (m_alarm == 1)){voltage1 = 0;}
  if (voltage2 > 0x2047){voltage2 = 0x2047;}
  if ((voltage2 < 0) || (m_alarm == 1)){voltage2 = 0;}
  if (voltage3 > 0x2047){voltage3 = 0x2047;}
  if ((voltage3 < 0) || (m_alarm == 1)){voltage3 = 0;}
  if (voltage4 > 0x2047){voltage4 = 0x2047;}
  if ((voltage4 < 0) || (m_alarm == 1)){voltage4 = 0;}
  /*---------- モーター電圧の平均 = 768 くらいで推力重力比が1になっていると思われる ------------*/
  throttleAverage = (voltage1 + voltage2 + voltage3 + voltage4)/4;
  if (throttleAverage > 768){
    integralOn = 1;                               // 積分制御をON ※使用していない
    digitalWrite(4,HIGH);                         // 青色LEDを点灯させるのみ
  } else {
    integralOn = 0;
    digitalWrite(4,LOW);
  }
  /*---------------------- 浮上していない状態でヨー角を初期化しておく -----------------------*/
  if (throttleAverage < 100){
    yawOn = 0;
  } else {
    yawOn = 1;
  }
  /*-------------- FPGA にモーター電圧値を出力する --------------------*/
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
  /*-------------- リモコンに電池電圧を送信する --------------------*/
  int lsb5 = sensorValue1 & 0x000F;
  int msb5 = sensorValue1 >> 4;
  s_dat[0] = 0x20;
  s_dat[1] = msb5;
  result = esp_now_send(slave.peer_addr, s_dat, sizeof(s_dat));
  s_dat[0] = 0x21;
  s_dat[1] = lsb5;
  result = esp_now_send(slave.peer_addr, s_dat, sizeof(s_dat));
}