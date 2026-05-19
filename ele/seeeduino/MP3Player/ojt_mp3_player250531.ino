
#include <Wire.h>                                           //  I2C
#include <ST7032.h>                                         //  LCD
#include <MsTimer2.h>                                       //  割込み用タイマー

ST7032 lcd;

char serial_dat[6][8]={ {0x7E,0xFF,0x06,0x11,0x00,0x00,0x00,0xEF},  //Reset
                        {0x7E,0xFF,0x06,0x0D,0x01,0x00,0x00,0xEF},  //Play
                        {0x7E,0xFF,0x06,0x0E,0x01,0x00,0x00,0xEF},  //Pause
                        {0x7E,0xFF,0x06,0x01,0x01,0x00,0x00,0xEF},  //Play Next
                        {0x7E,0xFF,0x06,0x02,0x01,0x00,0x00,0xEF},  //Play Previous
                        {0x7E,0xFF,0x06,0x06,0x01,0x00,0x05,0xEF}}; //Set Volume

int active_key6;
byte key_on6;
int active_key7;
byte key_on7;
int AD3_Value = 0;
int waitTimerCnt = 0;
  /* --------------------------------- 初期設定 --------------------------------------*/
void setup() {                                              // 初期設定
  Serial.begin(9600);
  AD3_Value = analogRead(A3)>>2;                            // VR情報を取得
  serial_dat[5][6]=(char)((AD3_Value*30)/256);
  for(int i=0;i<8;i++){
    Serial.write(serial_dat[5][i]);                         // VRを0に設定
  }
  pinMode(4,INPUT);                                         // sw4～sw7のポートを入力に設定
  pinMode(5,INPUT);                                         //
  pinMode(6,INPUT);                                         //
  key_on6 = 0;                                              //
  active_key6 = 0;                                          //
  pinMode(7,INPUT);                                         //
  key_on7 = 0;                                              //
  active_key7 = 0;                                          //

  pinMode(8,OUTPUT);                                        // LED8～LED11のポートを出力に設定
  pinMode(9,OUTPUT);                                        //
  pinMode(10,OUTPUT);                                       //
  pinMode(11,OUTPUT);                                       //

  lcd.begin(16, 2);                                         // LCD初期化　16x2
  lcd.setContrast(40);                                      //
  lcd.print("hello, world!");                               //
  
  MsTimer2::set(10, flash);                         // 10ms 間隔でタイマー割込みをセット。割込みが発生したらflash()という関数を実行する。
  MsTimer2::start();                                        // タイマー割込み開始。
}
  /* ------------------------------- メインループ　 --------------------------------------*/
void loop() {                                       // ---------- LCD表示 ----------------
  lcd.setCursor(12, 1);                                     // X:14,Y:1 の位置にカーソルを移動
  lcd.print("   ");                                           // 表示された値を一旦消去
  lcd.setCursor(12, 1);                                     // X:14,Y:1 の位置にカーソルを移動
  lcd.print(AD3_Value);                                         // カウンターの値を表示
  
  if(waitTimerCnt==0){                              // ---------- VR制御 -----------------
    AD3_Value = analogRead(A3)>>2;                          // 12bitのADCなので4bitシフトして8bitに。
    serial_dat[5][6]=(char)((AD3_Value*30)/256);            // 0～30の変化幅に変換
    for(int i=0;i<8;i++){
      Serial.write(serial_dat[5][i]);                       // UARTからVR情報を送信
    }
  }
  waitTimerCnt++;                                           // 10msを20回カウント:200msに一回処理
  if(waitTimerCnt>20){waitTimerCnt=0;}                      // ----------------------------

  delay(10);                                       // 10msのウエイト:loop一周は最小10ms
}

  /* ------------------------------ タイマー割込み -------------------------------------*/
void flash()                                                // タイマー割込みで実行される関数
{ 
  int port4 = digitalRead(4);                      // ----------- sw4押下処理 ------------
  digitalWrite(8,!port4);                                   // SW4が押されていたらLED8を点灯
  
  int port5 = digitalRead(5);                      // ----------- sw5押下処理 ------------
  digitalWrite(9,!port5);                                   // SW5が押されていたらLED9を点灯

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
    for(int i=0;i<8;i++){                                   // ここに処理を書く
      Serial.write(serial_dat[1][i]);                       // dfplayer再生
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
    for(int i=0;i<8;i++){                                   // ここに処理を書く
      Serial.write(serial_dat[0][i]);                       // dfplayer停止
    }
  }
}

  /* ----------------------------- シリアル通信割込み ----------------------------------*/
void serialEvent(){
}