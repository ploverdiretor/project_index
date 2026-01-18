#include <esp_now.h>
#include <WiFi.h>

//HardwareSerial mySerial1(2);

esp_now_peer_info_t slave;

const int ledPin = 32; //A4
const int Forward = 1;
const int Reverse = 2;
const int Stop = 0;
const int powFL = 33; //A5
const int powFR = 25; //A18
const int powBL = 26; //A19
const int powBR = 27; //A17
int wifi_throttle = 0;
int wifi_rudder = 0;
int wifi_pitch = 0;
int wifi_roll = 0;
unsigned char wifi_sensor = 0x00;
int front_sensor = 0;
int back_sensor = 0;
int right_sensor = 0;
int left_sensor = 0;
int motorL[129]={180,180,181,181,182,182,183,183,184,184,185,185,186,186,187,187,188,188,189,189,190,190,191,191,192,192,193,193,194,194,195,195,196,196,197,197,198,198,199,199,200,200,201,201,202,202,203,203,204,204,205,205,206,206,207,207,208,208,209,209,210,210,211,211,212,212,213,213,214,214,215,215,216,216,217,217,218,218,219,219,220,220,221,221,222,222,223,223,224,224,225,225,226,226,227,227,228,228,229,229,230,230,231,232,233,234,235,236,237,238,239,240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255,255,255};
int motor[64]={
180,181,182,184,185,186,187,188,189,191,
192,193,194,195,196,198,199,200,201,202,
203,205,206,207,208,209,210,212,213,214,
215,216,218,219,220,221,222,223,225,226,
227,228,229,230,232,233,234,235,236,237,
239,240,241,242,243,244,246,247,248,249,250,251,253,254};
static uint8_t brightness = 0;
static int diff = 2;
hw_timer_t* timer0 = NULL;

void IRAM_ATTR onTimer0(){
  ledcWrite(0, brightness);
  if (brightness == 0) {
    diff = 2;
  } else if (brightness == 254) {
    diff = -2;
  }
  brightness += diff;
  //遅延を遅くすればゆっくり明るさが変わるようになる
  
}

void OnDataRecv(const uint8_t *mac_addr, const unsigned char* data, int data_len){
  switch(data[0]){
    case 0x01:
      wifi_throttle = (int)data[1]-64;
      break;
    case 0x02:
      wifi_rudder = (int)data[1]-64;
      break;
    case 0x03:
      wifi_pitch = (int)data[1]-64;
      break;
    case 0x04:
      wifi_roll = (int)data[1]-64;
      break;
    case 0x0B:
      wifi_sensor = (unsigned char)data[1];
      break;
    default:
      break;      
  }
}

int sign(int A){
    if(A>0) return 1;
    else if(A<0) return -1;
    else return 0;
}

void spin_FL(int modeR, int power) {
  switch(modeR){
    case 0:
      digitalWrite(23, 0);
      digitalWrite(22, 0);
      ledcWrite(1, 0);
      break;
    case 1:   //Forward
      digitalWrite(23, 1);
      digitalWrite(22, 0);
      ledcWrite(1, power);
      break;
    case -1:   //Reverse
      digitalWrite(23, 0);
      digitalWrite(22, 1);
      ledcWrite(1, power);
      break;
  }
}
void spin_FR(int modeR, int power) {
  switch(modeR){
    case 0:
      digitalWrite(18, 0);
      digitalWrite(19, 0);
      ledcWrite(2, 0);
      break;
    case 1:
      digitalWrite(18, 1);
      digitalWrite(19, 0);
      ledcWrite(2, power);
      break;
    case -1:
      digitalWrite(18, 0);
      digitalWrite(19, 1);
      ledcWrite(2, power);
      break;
  }
}
void spin_BL(int modeR, int power) {
  switch(modeR){
    case 0:
      digitalWrite(5, 0);
      digitalWrite(0, 0);
      ledcWrite(3, 0);
      break;
    case 1:
      digitalWrite(5, 1);
      digitalWrite(0, 0);
      ledcWrite(3, power);
      break;
    case -1:
      digitalWrite(5, 0);
      digitalWrite(0, 1);
      ledcWrite(3, power);
      break;
  }
}
void spin_BR(int modeR, int power) {
  switch(modeR){
    case 0:
      digitalWrite(2, 0);
      digitalWrite(4, 0);
      ledcWrite(4, 0);
      break;
    case 1:
      digitalWrite(2, 1);
      digitalWrite(4, 0);
      ledcWrite(4, power);
      break;
    case -1:
      digitalWrite(2, 0);
      digitalWrite(4, 1);
      ledcWrite(4, power);
      break;
  }
}
void setup() {
  Serial.begin(115200);
//  mySerial1.begin(115200);//RX=16, TX=17
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
  ledcSetup(0, 12800, 8);  // 使用するタイマーのチャネルと周波数を設定
  ledcSetup(1, 12800, 8);  // 使用するタイマーのチャネルと周波数を設定
  ledcSetup(2, 12800, 8);  // 使用するタイマーのチャネルと周波数を設定
  ledcSetup(3, 12800, 8);  // 使用するタイマーのチャネルと周波数を設定
  ledcSetup(4, 12800, 8);  // 使用するタイマーのチャネルと周波数を設定
  ledcAttachPin(ledPin, 0);  // ledPinをチャネル0へ接続
  ledcAttachPin(powFL, 1);  // powFLをチャネル1へ接続
  ledcAttachPin(powFR, 2);  // powFRをチャネル1へ接続
  ledcAttachPin(powBL, 3);  // powBLをチャネル1へ接続
  ledcAttachPin(powBR, 4);  // powBRをチャネル1へ接続
  pinMode(23, OUTPUT); // pin23を出力モードにする
  pinMode(22, OUTPUT); // pin22を出力モードにする
  pinMode(19, OUTPUT); // pin19を出力モードにする
  pinMode(18, OUTPUT); // pin18を出力モードにする
  pinMode(5, OUTPUT); // pin5を出力モードにする
  pinMode(0, OUTPUT); // pin17を出力モードにする
  pinMode(2, OUTPUT); // pin16を出力モードにする
  pinMode(4, OUTPUT); // pin4を出力モードにする
  pinMode(35, INPUT); // pin35を出力モードにする
//  timer0 = timerBegin(1000000);
//  timerAttachInterrupt(timer0, &onTimer0);
//  timerAlarm(timer0,10000,true,0);
  timer0 = timerBegin(0, 80, true);
  timerAttachInterrupt(timer0, &onTimer0, true);
  timerAlarmWrite(timer0, 10000, true);
  timerAlarmEnable(timer0);
} 
void loop() {
  int powerMode = digitalRead(35);
  front_sensor = (int)((wifi_sensor>>3)&(0x01));
  back_sensor = (int)((wifi_sensor>>2)&(0x01));
  right_sensor = (int)((wifi_sensor>>1)&(0x01));
  left_sensor = (int)((wifi_sensor)&(0x01));
  int motorFL = (int)((wifi_pitch * 1 + wifi_roll * 1    + wifi_rudder * 1)/3);
  int motorFR = (int)((wifi_pitch * 1 + wifi_roll * (-1) + wifi_rudder * (-1))/3);
  int motorBL = (int)((wifi_pitch * 1 + wifi_roll * (-1) + wifi_rudder * 1)/3);
  int motorBR = (int)((wifi_pitch * 1 + wifi_roll * 1    + wifi_rudder * (-1))/3);
  if ((int)wifi_sensor != 0){
    int sensorFB = front_sensor * 16 - back_sensor * 16;
    int sensorLR = right_sensor * 16 - left_sensor * 16;
    motorFL = motorFL /2 + sensorFB - sensorLR;
    motorFR = motorFR /2 + sensorFB + sensorLR;
    motorBL = motorBL /2 + sensorFB + sensorLR;
    motorBR = motorBR /2 + sensorFB - sensorLR;
  }
  if(((front_sensor == 1)&&(back_sensor == 1))||((right_sensor == 1)&&(left_sensor == 1))){
    motorFL = 0;
    motorFR = 0;
    motorBL = 0;
    motorBR = 0;
  }
  if (powerMode == 0){
    if (sign(motorFL == 0)){
      spin_FL(Stop,0);
    } else {
      spin_FL(sign(motorFL),motor[abs(motorFL)]);
    }
    if (sign(motorFR == 0)){
      spin_FR(Stop,0);
    } else {
      spin_FR(sign(motorFR),motor[abs(motorFR)]);
    }
    if (sign(motorBL == 0)){
      spin_BL(Stop,0);
    } else {
      spin_BL(sign(motorBL),motor[abs(motorBL)]);
    }
    if (sign(motorBR == 0)){
      spin_BR(Stop,0);
    } else {
      spin_BR(sign(motorBR),motor[abs(motorBR)]);
    }
  }
  delay(10);
}
