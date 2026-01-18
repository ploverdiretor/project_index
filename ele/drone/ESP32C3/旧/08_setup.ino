void bldc_drv(uint8_t bldc_cmd, uint8_t bldc_dat){
  Wire.beginTransmission(DEVICE_ADDRESS);
    Wire.write(bldc_cmd);  // 
    Wire.write(bldc_dat);  // BLDC_ch1 
  Wire.endTransmission();
}

void setup(void)
{
  //--------------------------------------ADC & I/O-------------
  analogReadResolution(12);
  pinMode(10,OUTPUT);
  pinMode(4,OUTPUT);
  //--------------------------------------UART & I2C------------
  Serial.begin(115200);
//  while (!Serial) delay(10);

  //----------------------------------------------------------------------wifi ---------
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  if (esp_now_init() == ESP_OK) {
    Serial.println("ESPNow Init Success");
  } else {
    Serial.println("ESPNow Init Failed");
    ESP.restart();
  }
  memset(&slave, 0, sizeof(slave));
  for (int i=0; i<6; ++i){
    slave.peer_addr[i] = (uint8_t)0xff;
  }
  esp_err_t addStatus = esp_now_add_peer(&slave);
  if (addStatus == ESP_OK){
    Serial.println("Pair success");
  }
  esp_now_register_recv_cb(OnDataRecv);
  delay(1000);
  //----------------------------------------bno055--------------
  if(!bno.begin()){
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
  delay(1000);
  bno.setExtCrystalUse(true);
  delay(1000);
  Wire.begin();
  digitalWrite(10,HIGH);
  //----------------------------------------------------------------------ESC  ---------
  Serial.println("Writing maximum output.");
  bldc_drv(0x9F,0x7F);
  bldc_drv(0xBF,0x7F);
  bldc_drv(0xDF,0x7F);
  bldc_drv(0xFF,0x7F);
  Serial.println("Wait 2 seconds.");
  delay(2000);
  Serial.println("Writing minimum output");
  bldc_drv(0x90,0x00);
  bldc_drv(0xB0,0x00);
  bldc_drv(0xD0,0x00);
  bldc_drv(0xF0,0x00);
  Serial.println("Wait 2 seconds.");
  delay(2000);
  Serial.println("Start Motor");
  bldc_drv(0x80,0x00);
  bldc_drv(0xA0,0x00);
  bldc_drv(0xC0,0x00);
  bldc_drv(0xE0,0x00); 
  Serial.println("Wait 3 seconds.");
  digitalWrite(10,LOW);
  delay(3000);
  Serial.println("You have control.");

  //---------------------------------------timer-----------------
  timer0 = timerBegin(1000000);
  timerAttachInterrupt(timer0, &onTimer0);
  timerAlarm(timer0,20000,true,0);
}