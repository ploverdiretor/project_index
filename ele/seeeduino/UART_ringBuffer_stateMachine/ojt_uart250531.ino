
#include <Wire.h>                                           //  I2C
#include <ST7032.h>                                         //  LCD
#include <MsTimer2.h>                                       //  割込み用タイマー

#define UART_BUF_SIZE 5                                     // リングバッファのサイズ

#define IDLE 0                                              // ステートマシンの状態定義
#define STATUS01 1                                          //
#define STATUS02 2                                          //


byte uartRingBuf[UART_BUF_SIZE];                            // リングバッファ
int uart_r_index = 0;                                       // 読み出し側インデックス
int uart_w_index = 0;                                       // 書き込み側インデックス

byte cmd_buf[5];                                            // ステートマシン用バッファ
int cmd_index = 0;                                          //
int cmd_status = IDLE;                                      //

ST7032 lcd;                                                 // LCD

  /* --------------------------------- 初期設定 --------------------------------------*/
void setup() {                                              // 初期設定
  Serial.begin(9600);                                       //シリアル通信ボーレート

  lcd.begin(16, 2);                                         // LCD初期化　16x2
  lcd.setContrast(40);                                      //
  lcd.print("hello, world!");                               //
  
  MsTimer2::set(10, flash);                         // 10ms 間隔でタイマー割込みをセット。割込みが発生したらflash()という関数を実行する。
  MsTimer2::start();                                        // タイマー割込み開始。
}
  /* ------------------------------- メインループ　 --------------------------------------*/
void loop() {                                       // 何もしない
  delay(10);                                                // 10msのウエイト:loop一周は最小10ms
}

  /* ------------------------------ タイマー割込み -------------------------------------*/
void flash()                                                // タイマー割込みで実行される関数
{ 
  if(uart_w_index != uart_r_index){                         // リングバッファにデータがあったら
    byte inputDat = uartRingBuf[uart_r_index];              // inputDatに代入
    uart_r_index++;
    if(uart_r_index >= UART_BUF_SIZE){
      uart_r_index=0;
    }
    Serial.write(inputDat);                                 // inputDatをUARTに送信

    switch(cmd_status){                           //　ここからステートマシン-------------------
      case IDLE:                                            // IDLE------
        if(inputDat==0x61){                                 //  "a"という文字だったら－＞STATUS01
          cmd_index = 0;                                    //  "a"という文字でなければ－＞IDLE、cmd_bufをクリア
          cmd_buf[cmd_index]=inputDat;
          cmd_index++;
          cmd_status = STATUS01;
        }else{
          cmd_status = IDLE;
          for(int i=0;i<5;i++){cmd_buf[i]=0;}
        }
        break;
      case STATUS01:                                        // STATUS01------
        if(inputDat==0x62){                                 //  "b"という文字だったら－＞STATUS02
          cmd_buf[cmd_index]=inputDat;                      //  "b"という文字でなければ－＞IDLE
          cmd_index++;
          cmd_status = STATUS02;
        }else{
          cmd_status = IDLE;
        }
        break;
      case STATUS02:                                        // STATUS02------
        if(inputDat==0x63){                                 //  "c"という文字だったら－＞"abc"と表示
          cmd_buf[cmd_index]=inputDat;                      //  －＞IDLEに戻る
          cmd_index++;
          cmd_status = IDLE;
          Serial.write("\n");
          for(int i=0;i<cmd_index;i++){
            Serial.write(cmd_buf[i]);
          }
          Serial.write("\n");
        }else{
          cmd_status = IDLE;
        }
        break;
      default:                                              // IDLE,STATUS01,STATUS02以外にはならないが
        cmd_status = IDLE;                                  // 一応定義する
        break;
      
    }
  }
}

  /* ----------------------------- シリアル通信割込み ----------------------------------*/
void serialEvent(){
  if (Serial.available()>0) {                               // 受信バッファにあるデータの個数が0より大きかったら
    uartRingBuf[uart_w_index] = (char)Serial.read();        // リングバッファに代入
    uart_w_index++;
    if(uart_w_index >= UART_BUF_SIZE){
      uart_w_index=0;
    }
  }
}