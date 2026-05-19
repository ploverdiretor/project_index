
#include <Wire.h>                                           //  I2C
#include <ST7032.h>                                         //  LCD
#include <MsTimer2.h>                                       //  割込み用タイマー

ST7032 lcd;

  /* --------------------------------- 初期設定 --------------------------------------*/
void setup() {                                              // 初期設定
  pinMode(4,INPUT);                                         // sw4のポートを入力に設定

  pinMode(8,OUTPUT);                                        // LED8のポートを出力に設定

  lcd.begin(16, 2);                                         // LCD初期化　16x2
  lcd.setContrast(40);                                      //
  lcd.print("hello, world!");                               //
  
}
  /* ------------------------------- メインループ　 --------------------------------------*/
void loop() {                                       
  int port4 = digitalRead(4);                      // ----------- sw4押下処理 ------------
  digitalWrite(8,!port4);                                   // SW4が押されていたらLED8を点灯

  lcd.setCursor(1, 1);                             // ---------- LCD表示 ----------------
  lcd.print("   ");                                           // 表示された値を一旦消去
  lcd.setCursor(1, 1);                                        // X:1,Y:1 の位置にカーソルを移動
  if(port4==0){
    lcd.print(" ON");                                         // ON表示
  }else{
    lcd.print("OFF");                                         // OFF表示
  }

  delay(10);                                       // 10msのウエイト:loop一周は最小10ms
}

