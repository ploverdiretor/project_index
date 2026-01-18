#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#define ap_pass "espcast_pass_0000"
#define BNO055_SAMPLERATE_DELAY_MS (20)
Adafruit_BNO055 bno = Adafruit_BNO055(-1, 0x28, &Wire);
uint8_t mac[6] = {0x58, 0xCF, 0x79, 0xF1, 0x75, 0x10}; //AP MAC
uint8_t wifi_data[]={0,0,0,0,0,0,0,0};
unsigned char s_dat[8];
esp_now_peer_info_t slave;
int vr_table[128] = {-64,-64,-64,-62,-60,-58,-56,-55,-54,-53,-52,-51,-50,-49,-48,-47,-46,-45,-44,-43,-42,-41,-40,-39,-38,-37,-36,-35,-34,-33,-32,-31,-30,-29,-28,-27,-26,-25,-24,-23,-22,-21,-20,-19,-18,-17,-16,-15,-14,-13,-12,-11,-10,-9,-8,-7,-6,-5,-4,-3,-2,-1,0,0,0,0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,57,59,61,63,63,63};

uint8_t DEVICE_ADDRESS = 0x01;  // 2進 100000

int wifi_pitch = 0;
int wifi_rudder = 0;
int wifi_throttle = 0;
int wifi_roll = 0;
int wifi_buttonL = 0;
int wifi_buttonR = 0;
int wifi_dip1 = 0;
int wifi_dip2 = 0;
int wifi_Active_Sense = 0;
int throttleAverage = 0;
int integralOn = 0;
int yawOn = 0;
double initYaw = 0;

double wifi_vr00 = 0;
double wifi_vr01 = 0;
double wifi_vr02 = 0;
double wifi_vr03 = 0;
double wifi_vr10 = 0;
double wifi_vr11 = 0;
double wifi_vr12 = 0;
double wifi_vr13 = 0;
double batt_value = 0;
/* Set the delay between fresh samples */

double batt_voltage = 0;

const int gyroNumReadings = 3;  // 平均化するデータポイントの数
double gyroXYZ[3][gyroNumReadings];   // データのバッファ
int gyroIndex = 0;               // 最新データを追加するためのインデックス
double gyroTotal[3] = {0,0,0};               // データの合計

int active_key = 0;
byte key_on = 0;

double degP = 0;
double degR = 0;
double degY = 0;
double gyroP = 0;
double gyroR = 0;
double gyroY = 0;

double pitch_Kp1 = 1.5;
double pitch_Ki1 = 0;
double pitch_Kp2 = 0;
double pitch_Ki2 = 0;
double pitch_Kd2 = 0;
double pitch_i_Limit = 20;
double pid_P2_Limit = 500;

double roll_Kp1 = pitch_Kp1;
double roll_Ki1 = pitch_Ki1;
double roll_Kp2 = pitch_Kp2;
double roll_Ki2 = pitch_Ki2;
double roll_Kd2 = pitch_Kd2;
double roll_i_Limit = pitch_i_Limit;
double pid_R2_Limit = pid_P2_Limit;

double yaw_Kp1 = pitch_Kp1;
double yaw_Kp2 = pitch_Kp2;
double yaw_Ki2 = pitch_Ki2;
double yaw_Kd2 = pitch_Kd2;
double yaw_i_Limit = pitch_i_Limit;
double pid_Y2_Limit = 200;

double preErrDegP = 0;
double pid_P1 = 0;
double pid_P2 = 0;
double preErrDegR = 0;
double pid_R1 = 0;
double pid_R2 = 0;
double preErrDegY = 0;
double pid_Y1 = 0;
double pid_Y2 = 0;
double refDegY = 0;
double errDegY = 0;
int m_alarm = 0;
unsigned short voltage1;
unsigned short voltage2;
unsigned short voltage3;
unsigned short voltage4;
uint8_t cmd;

int i2c_active = 0;
int AS_counter = 0;
hw_timer_t* timer0 = NULL;

void IRAM_ATTR onTimer0() {
  i2c_active = 1;
  AS_counter = AS_counter + 1;
  if (AS_counter > 50){
    wifi_Active_Sense = 0;
    AS_counter = 0;
  }
}
void OnDataRecv(const esp_now_recv_info* mac_addr, const unsigned char* data, int data_len) {
  switch(data[0]){
    case 0x00: //----------------------------------Active Sense--------------------
      wifi_Active_Sense = (int)data[1];
      AS_counter = 0;
      break;
    case 0x01: //----------------------------------スロットル--------------------
      wifi_throttle = ((int)data[1]-64)*2.5;
      break;
    case 0x02: //----------------------------------ラダー--------------------
      wifi_rudder = ((int)data[1]-64)*2;
      break;
    case 0x03: //----------------------------------ピッチ--------------------
      wifi_pitch = ((int)data[1]-64)*2;
      break;
    case 0x04: //----------------------------------ロール--------------------
      wifi_roll = ((int)data[1]-64)*(-2); 
      break;
    case 0x05: //----------------------------------SW-L--------------------
      wifi_buttonL = (int)data[1]; 
      break;
    case 0x06: //----------------------------------SW-R--------------------
      wifi_buttonR = (int)data[1]; 
      break;
    case 0x07: //----------------------------------DIP-1--------------------
      wifi_dip1 = (int)data[1]; 
      break;
    case 0x08: //----------------------------------DIP-2--------------------
      wifi_dip2 = (int)data[1];  
      break; 
    case 0x10: //----------------------------------半固定00--------------------
      wifi_vr00 = vr_table[(int)data[1]]; 
      break;
    case 0x11: //----------------------------------半固定01--------------------
      wifi_vr01 = (int)data[1]; 
      break;
    case 0x12: //----------------------------------半固定02--------------------
      wifi_vr02 = (int)data[1]; 
      break;
    case 0x13: //----------------------------------半固定03--------------------
      wifi_vr03 = (int)data[1]; 
      break;
    case 0x14: //----------------------------------半固定10--------------------
      wifi_vr10 = vr_table[(int)data[1]]; 
      break;
    case 0x15: //----------------------------------半固定11--------------------
      wifi_vr11 = (int)data[1]; 
      break;
    case 0x16: //----------------------------------半固定12--------------------
      wifi_vr12 = (int)data[1]; 
      break;
    case 0x17: //----------------------------------半固定13--------------------
      wifi_vr13 = (int)data[1]; 
      break;
    default:
      break;
  }
}