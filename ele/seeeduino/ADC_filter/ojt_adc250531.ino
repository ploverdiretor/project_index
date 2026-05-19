#include <Wire.h>                                           //  I2C
#include <ST7032.h>                                         //  LCD
#include <MsTimer2.h>                                       //  割込み用タイマー

ST7032 lcd;

int AD3_Value = 0;

byte  mafBuf[100];
int   mafIndex = 0;
short mafTotal = 0;
byte  mafOut = 0;
int numBuf=10;
int numFilter = 10;

int active_key6=0;
byte key_on6=0;
int active_key7=0;
byte key_on7=0;

  /* --------------------------------- 初期設定 --------------------------------------*/
void setup() {                                              // 初期設定
  Serial.begin(9600);
  AD3_Value = analogRead(A3)>>2;                            // VR情報を取得
  for(int i=0;i<100;i++){
    mafBuf[i]=0;
  }
  pinMode(6,INPUT);                                         // sw4～sw7のポートを入力に設定
  pinMode(7,INPUT);                                         //

  pinMode(10,OUTPUT);                                       // LED8～LED11のポートを出力に設定
  pinMode(11,OUTPUT);                                       //

  lcd.begin(16, 2);                                         // LCD初期化　16x2
  lcd.setContrast(40);                                      //
  lcd.print("Mov-Ave-Filters");                               //
  
  MsTimer2::set(10, flash);                         // 10ms 間隔でタイマー割込みをセット。割込みが発生したらflash()という関数を実行する。
  MsTimer2::start();                                        // タイマー割込み開始。
}
  /* ------------------------------- メインループ　 --------------------------------------*/
void loop() {                                       // ---------- LCD表示 ----------------
  lcd.setCursor(9, 1);                                     // X:14,Y:1 の位置にカーソルを移動
  lcd.print("val:   ");                                           // 表示された値を一旦消去
  lcd.setCursor(13, 1);                                     // X:14,Y:1 の位置にカーソルを移動
  lcd.print(mafOut);                                         // カウンターの値を表示
  lcd.setCursor(0, 1);                                     // X:14,Y:1 の位置にカーソルを移動
  lcd.print("num:   ");                                           // 表示された値を一旦消去
  lcd.setCursor(4, 1);                                     // X:14,Y:1 の位置にカーソルを移動
  lcd.print(numFilter);                                         // カウンターの値を表示

  delay(1);                                         // 10msのウエイト:loop一周は最小10ms
}

  /* ------------------------------ タイマー割込み -------------------------------------*/
void flash()                                                // タイマー割込みで実行される関数
{ 
  if(mafIndex==0){
    numBuf = numFilter;
  }
  AD3_Value = analogRead(A3)>>2; 
  mafTotal = mafTotal - (short)mafBuf[mafIndex];                   // 古いデータの合計から最も古いデータを減算
  mafBuf[mafIndex] = AD3_Value;                             // 新しいデータをバッファに追加
  mafTotal = mafTotal + (short)mafBuf[mafIndex];                   // 新しいデータを合計に加算
  if (mafIndex >= (numBuf - 1)){
    mafIndex = 0;
  } else {
    mafIndex += 1;
  }
  mafOut = (byte)(mafTotal / numBuf);  // 平均値を計算

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
    if(numFilter<=5){
      numFilter=5;
    }else{
      numFilter--;
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
    if(numFilter>=99){
      numFilter=99;
    }else{
      numFilter++;
    }
  }
}

  /* ----------------------------- シリアル通信割込み ----------------------------------*/
void serialEvent(){
}