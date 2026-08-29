#include <Arduino.h>
#include <DShotRMT.h>

// ピン定義
#define ESC_PIN GPIO_NUM_18    // 基板右側・上から4番目 (D10)
#define POT_PIN GPIO_NUM_1     // 基板左側・上から1番目 (D0) -> 可変抵抗器の真ん中ピン

// DShotRMTインスタンスの作成
DShotRMT esc(ESC_PIN, DSHOT600, false);

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("DShot600 Potentiometer Control Start");

  // アナログ入力ピンの設定
  pinMode(POT_PIN, INPUT);

  // DShotRMTの初期化
  esc.begin();
  Serial.println("DShot RMT Driver initialized.");

  // ESCのアーミング（安全ロック解除）
  Serial.println("Arming ESC... Please wait.");
  for (int i = 0; i < 200; i++) {
    esc.sendThrottle(0); // スロットル0を送信
    delay(10);           // 約2秒間
  }
  Serial.println("ESC Armed and Ready!");
}

void loop() {
  // 1. 可変抵抗器の電圧（アナログ値）を読み取る (0 〜 4093)
  int potValue = analogRead(POT_PIN);

  // 2. アナログ値をDShotの有効範囲に変換（マッピング）
  // つまみが最小(0)のときはDShot値「0」（モーター停止）
  // つまみが少しでも動いたら、DShotの最低回転値「48」〜最高回転値「2047」へ滑らかに繋ぎます
  uint16_t throttle = 0;
  if (potValue > 50) { // ノイズによる誤動作を防ぐ不感帯(デッドバンド)を設定
    // map(入力値, 入力最小, 入力最大, 出力最小, 出力最大)
    throttle = map(potValue, 50, 4093, 48, 2047);
  } else {
    throttle = 0;
  }

  // 3. ESCにスロットル信号を送信
  esc.sendThrottle(throttle);

  // 4. シリアルモニターに現在の状態を表示（確認用）
  Serial.print("Pot: ");
  Serial.print(potValue);
  Serial.print(" -> Throttle: ");
  Serial.println(throttle);

  delay(20); // 制御周期を約50Hz（20ms）に維持
}
