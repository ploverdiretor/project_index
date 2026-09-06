#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>

// I2Cピン定義（ESP32-C6）
#define I2C_SDA 6
#define I2C_SCL 7

Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28, &Wire);

// 制御周期 (100Hz = 10ms)
const unsigned long SAMPLE_TIME_MS = 10;
unsigned long lastTime = 0;

// --- 3軸PIDゲイン（機体に合わせて要調整） ---
// アウターループ（姿勢角 Pゲイン）
float kp_ang_roll  = 4.0;
float kp_ang_pitch = 4.0;
float kp_ang_yaw   = 3.0;

// インナーループ（角速度 PIDゲイン）
float kp_rate_roll = 1.5;  float ki_rate_roll = 0.05; float kd_rate_roll = 0.02;
float kp_rate_pitch = 1.5; float ki_rate_pitch = 0.05; float kd_rate_pitch = 0.02;
float kp_rate_yaw = 2.5;   float ki_rate_yaw = 0.01;  float kd_rate_yaw = 0.00;

// 制限値
const float RATE_LIMIT = 150.0;   // 目標角速度の上限 (deg/s)
const float MOTOR_LIMIT = 500.0;  // 各軸のミキシング量上限
const float I_TERM_LIMIT = 100.0; // インナーループI項のアンチワインドアップ上限

// インナーループ（角速度）の過去データ保持用
float err_sum_r = 0, last_err_r = 0;
float err_sum_p = 0, last_err_p = 0;
float err_sum_y = 0, last_err_y = 0;

// プロポ（受信機）からの目標角度入力をシミュレートする変数（ラジアン）
float cmd_roll  = 0.0; 
float cmd_pitch = 0.0;
float cmd_yaw   = 0.0; 

// オイラー角（ラジアン）からクォータニオンへの変換関数
imu::Quaternion euler_to_quaternion(float roll, float pitch, float yaw) {
  float cr = cos(roll * 0.5);
  float sr = sin(roll * 0.5);
  float cp = cos(pitch * 0.5);
  float sp = sin(pitch * 0.5);
  float cy = cos(yaw * 0.5);
  float sy = sin(yaw * 0.5);

  imu::Quaternion q;
  q.w() = cr * cp * cy + sr * sp * sy;
  q.x() = sr * cp * cy - cr * sp * sy;
  q.y() = cr * sp * cy + sr * cp * sy;
  q.z() = cr * cp * sy - sr * sp * cy;
  return q;
}

void setup() {
  Serial.begin(115200);
  Wire.begin(I2C_SDA, I2C_SCL);

  if (!bno.begin()) {
    Serial.println("BNO055が見つかりません");
    while (1);
  }
  bno.setExtCrystalUse(true);
  lastTime = millis();
}

void loop() {
  unsigned long currentTime = millis();
  unsigned long elapsedTime = currentTime - lastTime;

  if (elapsedTime >= SAMPLE_TIME_MS) {
    float dt = (float)elapsedTime / 1000.0;

    // ==========================================
    // 0. プロポ入力の読み込み（シミュレート値）
    // ==========================================
    // 実際にはここで `pulseIn()` や SBUS、CRSF等の受信機データから
    // 目標角度（ラジアン変換後）を代入してください。
    cmd_roll  = 0.0; // 例: 0度
    cmd_pitch = 0.0; 
    cmd_yaw   = 0.0; 

    // ==========================================
    // 1. センサーデータ（クォータニオン＆角速度）取得
    // ==========================================
    imu::Quaternion q_curr = bno.getQuat();
    imu::Vector<3> gyro = bno.getVector(Adafruit_BNO055::VECTOR_GYRO);
    
    // BNO055の配置方向に応じて軸と符号を合わせてください
    float rate_curr_roll  = gyro.x(); 
    float rate_curr_pitch = gyro.y();
    float rate_curr_yaw   = gyro.z();

    // ==========================================
    // 2. アウターループ（クォータニオン姿勢制御）
    // ==========================================
    // プロポの目標オイラー角を目標クォータニオンへ変換
    imu::Quaternion q_target = euler_to_quaternion(cmd_roll, cmd_pitch, cmd_yaw);

    // 誤差クォータニオン計算 (q_error = q_target * q_curr_conjugate)
    imu::Quaternion q_curr_conj = q_curr.conjugate();
    imu::Quaternion q_error = q_target * q_curr_conj;

    // クォータニオンの回転方向を最短ルートに補正（一意性の確保）
    if (q_error.w() < 0.0) {
      q_error.w() = -q_error.w();
      q_error.x() = -q_error.x();
      q_error.y() = -q_error.y();
      q_error.z() = -q_error.z();
    }

    // クォータニオンのベクトル成分(x,y,z)から目標角速度(deg/s)を算出
    // ※微小角において、ベクトル成分は各軸の角度エラーに比例します
    float target_rate_roll  = kp_ang_roll  * q_error.x() * 2.0 * (180.0 / M_PI);
    float target_rate_pitch = kp_ang_pitch * q_error.y() * 2.0 * (180.0 / M_PI);
    float target_rate_yaw   = kp_ang_yaw   * q_error.z() * 2.0 * (180.0 / M_PI);

    // 目標角速度をサチュレーション制限
    target_rate_roll  = constrain(target_rate_roll,  -RATE_LIMIT, RATE_LIMIT);
    target_rate_pitch = constrain(target_rate_pitch, -RATE_LIMIT, RATE_LIMIT);
    target_rate_yaw   = constrain(target_rate_yaw,   -RATE_LIMIT, RATE_LIMIT);

    // ==========================================
    // 3. インナーループ（角速度PID制御）
    // ==========================================
    // --- ロール軸 ---
    float err_r = target_rate_roll - rate_curr_roll;
    err_sum_r = constrain(err_sum_r + (err_r * dt), -I_TERM_LIMIT, I_TERM_LIMIT);
    float d_err_r = (err_r - last_err_r) / dt;
    float out_roll = (kp_rate_roll * err_r) + (ki_rate_roll * err_sum_r) + (kd_rate_roll * d_err_r);
    last_err_r = err_r;

    // --- ピッチ軸 ---
    float err_p = target_rate_pitch - rate_curr_pitch;
    err_sum_p = constrain(err_sum_p + (err_p * dt), -I_TERM_LIMIT, I_TERM_LIMIT);
    float d_err_p = (err_p - last_err_p) / dt;
    float out_pitch = (kp_rate_pitch * err_p) + (ki_rate_pitch * err_sum_p) + (kd_rate_pitch * d_err_p);
    last_err_p = err_p;

    // --- ヨー軸 ---
    float err_y = target_rate_yaw - rate_curr_yaw;
    err_sum_y = constrain(err_sum_y + (err_y * dt), -I_TERM_LIMIT, I_TERM_LIMIT);
    float d_err_y = (err_y - last_err_y) / dt;
    float out_yaw = (kp_rate_yaw * err_y) + (ki_rate_yaw * err_sum_y) + (kd_rate_yaw * d_err_y);
    last_err_y = err_y;

    // 各出力をリミット内に収める
    out_roll  = constrain(out_roll,  -MOTOR_LIMIT, MOTOR_LIMIT);
    out_pitch = constrain(out_pitch, -MOTOR_LIMIT, MOTOR_LIMIT);
    out_yaw   = constrain(out_yaw,   -MOTOR_LIMIT, MOTOR_LIMIT);

    // ==========================================
    // 4. モーターミキシング（クアッドコプター X配置の例）
    // ==========================================
    // 実際には throttle（プロポの推力）の値をベースに足し引きします。
    float throttle = 1500; // ダミーの基準スロットル値 (PWM 1000?2000想定)

    // 各モーターへの出力計算 (ミキシングマップ)
    float m1 = throttle + out_pitch - out_roll + out_yaw; // 前右
    float m2 = throttle + out_pitch + out_roll - out_yaw; // 前左
    float m3 = throttle - out_pitch - out_roll - out_yaw; // 後右
    float m4 = throttle - out_pitch + out_roll + out_yaw; // 後左

    // ※ ここで m1~m4 の値を `ledcWrite()` や `analogWrite()` 等で
    // 各ESC（モータードライバ）にPWM/OneShot信号として出力します。

    // デバッグ表示
    static int count = 0;
    if (count++ % 10 == 0) { // 100msごとに間引いて表示
      Serial.print("Roll_Err:"); Serial.print(q_error.x());
      Serial.print("\tPitch_Err:"); Serial.print(q_error.y());
      Serial.print("\tOut_R:"); Serial.print(out_roll);
      Serial.print("\tOut_P:"); Serial.println(out_pitch);
    }

    lastTime = currentTime;
  }
}
