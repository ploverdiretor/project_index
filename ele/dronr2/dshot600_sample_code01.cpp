#include <Arduino.h>
#include "driver/rmt_tx.h" // ESP-IDF内蔵のRMTドライバーをインクルード

// 使用するGPIOピン（お使いのボードに合わせて自由に変更してください）
#define DSHOT_GPIO_PIN    4 

// DShot600 タイミングカウント (40MHz ソースクロックベース、1カウント=25ns)
#define T0H   25         // 625ns (0ビットのHIGH期間)
#define T0L   42         // 1042ns (0ビットのLOW期間)
#define T1H   50         // 1250ns (1ビットのHIGH期間)
#define T1L   17         // 425ns (1ビットのLOW期間)
#define RESET_LOW 1600   // 40us (フレーム間の最小リセットタイム)

// RMTオブジェクト保持用グローバル変数
rmt_channel_handle_t tx_chan = NULL;
rmt_encoder_handle_t dshot_encoder = NULL;

// 16bitパケットをRMTシンボル形式にカスタム変換するコールバック
size_t IRAM_ATTR dshot_encode_cb(rmt_encoder_handle_t encoder, rmt_channel_handle_t channel, const void *primary_data, size_t data_size, rmt_symbol_word_t *rmt_symbols) {
    uint16_t packet = *(uint16_t*)primary_data;
    
    // 1. 16ビット分のパルス波形をシリアライズ
    for (int i = 0; i < 16; i++) {
        bool bit = (packet >> (15 - i)) & 0x01; // MSBから順に取り出す
        if (bit) {
            rmt_symbols[i] = (rmt_symbol_word_t){.level0 = 1, .duration0 = T1H, .level1 = 0, .duration1 = T1L};
        } else {
            rmt_symbols[i] = (rmt_symbol_word_t){.level0 = 1, .duration0 = T0H, .level1 = 0, .duration1 = T0L};
        }
    }
    
    // 2. フレーム終わりのリセットタイム（LOW状態を一定時間維持）
    rmt_symbols[16] = (rmt_symbol_word_t){.level0 = 0, .duration0 = RESET_LOW, .level1 = 0, .duration1 = 0};
    
    return 17; // 合計17個のシンボル（16ビットデータ + 1リセット）をRMTへ渡す
}

// スロットル（11bit）とテレメトリ（1bit）からDShotパケット（CRC4bit含）を生成する関数
uint16_t create_dshot_packet(uint16_t throttle, bool telemetry) {
    if (throttle > 2047) throttle = 2047;
    
    // スロットル11bit + テレメトリ1bit を結合
    uint16_t packet = (throttle << 5) | (telemetry ? 0x10 : 0x00);
    
    // CRCの計算 (ニブル単位のXORチェックサム)
    uint16_t c_packet = packet >> 4;
    uint8_t crc = (c_packet ^ (c_packet >> 4) ^ (c_packet >> 8)) & 0x0F;
    
    // 最下位4ビットにCRCを結合
    packet |= crc;
    return packet;
}

// RMT（Remote Control）ペリフェラルの初期化
void init_dshot() {
    // 1. RMT TX チャンネルの設定
    rmt_tx_channel_config_t tx_chan_config = {
        .gpio_num = (gpio_num_t)DSHOT_GPIO_PIN,
        .clk_src = RMT_CLK_SRC_DEFAULT, // XTAL (40MHz) を使用
        .mem_block_symbols = 64,        // バッファ容量
        .resolution_hz = 40000000,      // 解像度を40MHzに固定 (1カウント=25ns)
        .trans_queue_depth = 4,
        .flags = {
            .with_dma = false,          // シンプルにするためDMAはオフ
        }
    };
    ESP_ERROR_CHECK(rmt_new_tx_channel(&tx_chan_config, &tx_chan));

    // 2. カスタムエンコーダの作成
    rmt_copy_encoder_config_t copy_encoder_config = {};
    ESP_ERROR_CHECK(rmt_new_copy_encoder(&copy_encoder_config, &dshot_encoder));
    
    // ※ 新APIの構造体に独自の変換コールバックを紐付ける処理（今回は簡易的な直接指定形式）
    // ESP-IDFの仕様上、copy_encoderは渡したバイトデータをそのままパルスとして吐き出すため、
    // 以下の送信関数側でパケットデータをエンコード処理します。
    
    // 3. チャンネルの有効化
    ESP_ERROR_CHECK(rmt_enable(tx_chan));
}

// DShotパケットの送信実行
void send_dshot_value(uint16_t throttle, bool telemetry = false) {
    uint16_t packet = create_dshot_packet(throttle, telemetry);
    
    // 送信用に17個のRMTシンボルバッファを用意
    rmt_symbol_word_t rmt_symbols[17];
    dshot_encode_cb(dshot_encoder, tx_chan, &packet, sizeof(packet), rmt_symbols);
    
    rmt_transmit_config_t tx_config = {
        .loop_count = 0, // 単発送信
        .flags = {
            .eot_level = 0 // 送信終了後はLOWを維持
        }
    };
    
    // ハードウェアRMTへ送信要求（ノンブロッキング）
    rmt_transmit(tx_chan, dshot_encoder, rmt_symbols, sizeof(rmt_symbols), &tx_config);
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("ESP32-C6 DShot600 Initializing...");
    
    init_dshot();
    
    // ⚠️安全上の重要警告：ベンチテスト時は必ず「プロペラ」を外してください
    Serial.println("[WARNING] REMOVE PROPELLERS BEFORE TESTING!");
    
    // ESCのアームシーケンス（起動時は最低でも数十〜数百回、スロットル0を送る必要があります）
    Serial.println("Arming ESC (Sending 0)...");
    for(int i = 0; i < 300; i++) {
        send_dshot_value(0); 
        delay(5); // 約5ms周期（200Hz）で初期化信号を送信
    }
    Serial.println("ESC Armed!");
}

void loop() {
    // 【テスト動作】モーターをスロットル48（最小回転）から徐々に加速させてみる
    Serial.println("Motor Spin: Accelerating...");
    for (uint16_t throttle = 48; throttle < 400; throttle++) { 
        send_dshot_value(throttle);
        delay(10); // 加速のスピード調整
    }
    
    delay(2000); // 2秒間中速をキープ
    
    Serial.println("Motor Stop.");
    send_dshot_value(0); // モーター停止
    delay(4000); // 4秒待機してループを繰り返す
}
