
/* チャタリング対策付スイッチ */

#include <Wire.h>                                           //  I2C
#include <ST7032.h>                                         //  LCD
#include <MsTimer2.h>                                       //  割込み用タイマー

ST7032 lcd;
int active_key6;
byte key_on6;
int active_key7;
byte key_on7;
byte count=0;
  /* --------------------------------- 初期設定 --------------------------------------*/
void setup() {                                              // 初期設定
  pinMode(6,INPUT);                                         //
  key_on6 = 0;                                              //
  active_key6 = 0;                                          //
  pinMode(7,INPUT);                                         //
  key_on7 = 0;                                              //
  active_key7 = 0;                                          //

  pinMode(10,OUTPUT);                                       //LED10～LED11のポートを出力に設定
  pinMode(11,OUTPUT);                                       //

  lcd.begin(16, 2);                                         // LCD初期化　16x2
  lcd.setContrast(40);                                      //
  lcd.print("hello, world!");                               //

  MsTimer2::set(10, flash);                         // 10ms 間隔でタイマー割込みをセット。割込みが発生したらflash()という関数を実行する。
  MsTimer2::start();                                        // タイマー割込み開始。
}
  /* ------------------------------- メインループ　 --------------------------------------*/
void loop() {                                       
  lcd.setCursor(1, 1);                             // ---------- LCD表示 ----------------
  lcd.print("   ");                                           // 表示された値を一旦消去
  lcd.setCursor(1, 1);                                        // X:1,Y:1 の位置にカーソルを移動
  lcd.print(count);                                         // ON表示

  delay(10);                                       // 10msのウエイト:loop一周は最小10ms
}

  /* ------------------------------ タイマー割込み -------------------------------------*/
void flash()                                                // タイマー割込みで実行される関数
{ 
  int port6 = digitalRead(6);                      // --------- sw6押下処理（チャタリング対策付）----------
  digitalWrite(10,!port6);                                  // SW6が押されていたらLED10を点灯
  key_on6 = key_on6 << 1;                                   // シフトレジスタを1ビットシフト
  if(port6 == 0){                                           // 押されていたらシフトレジスタの最下位を1にする
    key_on6 = key_on6 + 1;
  }else{                                                    // 押されていなかったら判定レジスタを0にする
    active_key6=0;
  }
  if(((key_on6&(byte)0x0F)==B0000111)&&(active_key6==0)){   // シフトレジスタが"0111"だったら押下されたと判定
    active_key6=1;                                          // 判定レジスタを1にすることで、一度しか実行されないようにする
    if(count>0){
      count--;                                              // countを-1
    }
  }

  int port7 = digitalRead(7);                      // --------- sw7押下処理（チャタリング対策付）----------
  digitalWrite(11,!port7);                                  // SW7が押されていたらLED11を点灯
  key_on7 = key_on7 << 1;                                   // シフトレジスタを1ビットシフト
  if(port7 == 0){                                           // 押されていたらシフトレジスタの最下位を1にする
    key_on7 = key_on7 + 1;
  }else{                                                    // 押されていなかったら判定レジスタを0にする
    active_key7=0;
  }
  if(((key_on7&(byte)0x0F)==B0000111)&&(active_key7==0)){   // シフトレジスタが"0111"だったら押下されたと判定
    active_key7=1;                                          // 判定レジスタを1にすることで、一度しか実行されないようにする
    if(count<10){                                       
      count++;                                              // countを+1
    }
  }
}