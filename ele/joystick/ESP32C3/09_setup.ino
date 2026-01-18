
String strPad(int num, int zeroCount){
  char tmp[256];
  char param[5] = {'%','0',(char)(zeroCount+48),'d','\0'};
  sprintf(tmp,param,num);
  return tmp;
}

void setup() {
  Wire.begin();
  lcd.begin(16,2);
  lcd.setContrast(40);
  Serial.begin(115200);
  pinMode(SS1, OUTPUT);
  pinMode(SS2, OUTPUT);
  digitalWrite(SS1, HIGH);
  digitalWrite(SS2, HIGH);
  SPI.begin();
  //--------------------------wifi----------------------------
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
}
