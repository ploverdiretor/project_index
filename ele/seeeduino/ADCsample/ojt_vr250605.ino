
#include <Wire.h>                                           //  I2C
#include <ST7032.h>                                         //  LCD
#include <MsTimer2.h>                                       //  割込み用タイマー

ST7032 lcd;

  /* --------------------------------- 初期設定 --------------------------------------*/
void setup() {                                              // 初期設定
  lcd.begin(16, 2);                                         // LCD初期化　16x2
  lcd.setContrast(40);                                      //
  lcd.print("hello, world!");                               //
  
}
  /* ------------------------------- メインループ　 --------------------------------------*/
void loop() {                                       
  byte AD3_Value = analogRead(A3)>>2;                       // AD3ポートを読む。10bitのADCなので2bitシフトして8bitにする。

  lcd.setCursor(1, 1);                             // ---------- LCD表示 ----------------
  lcd.print("   ");                                         // 一旦表示を消す
  lcd.setCursor(1, 1);                                      // X:1,Y:1 の位置にカーソルを移動
  lcd.print(AD3_Value);                                     // 値を表示

  delay(10);                                       // 10msのウエイト:loop一周は最小10ms
}

