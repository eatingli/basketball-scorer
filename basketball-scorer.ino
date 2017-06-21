#define SEG7_FLASH_PERIOD 3
#define MAX_SCORE 999
#define ADD_SCORE 2
#define GAMEOVER_PERIOD 6000
#define DEBONUCE_PERIOD 25
#define BTN_PIN 12
#define BUZZER_PIN 13

// 定義七段顯示器 A~G 腳位
const byte SEG7_LED_PIN[7] = {5, 7, 8, 9, 11, 6, 10};

// 定義七段顯示器 0~2位置腳
const byte SEG7_POSITION_PIN[3] = {3, 2, 4};

// 定義每個"位置"對應的電位狀態
const boolean POSITION[3][3] = {
  {0, 1, 1}, // 0
  {1, 0, 1}, // 1
  {1, 1, 0}, // 2
};

// 定義每個"數字"對應的電位狀態
const boolean NUMBER[10][7] = {
  {1, 1, 1, 1, 1, 1, 0}, // 0
  {0, 1, 1, 0, 0, 0, 0}, // 1
  {1, 1, 0, 1, 1, 0, 1}, // 2
  {1, 1, 1, 1, 0, 0, 1}, // 3
  {0, 1, 1, 0, 0, 1, 1}, // 4
  {1, 0, 1, 1, 0, 1, 1}, // 5
  {1, 0, 1, 1, 1, 1, 1}, // 6
  {1, 1, 1, 0, 0, 0, 0}, // 7
  {1, 1, 1, 1, 1, 1, 1}, // 8
  {1, 1, 1, 1, 0, 1, 1}, // 9
};

/*-----------------------------------------------------------*/
/*
  全域變數
*/
long nowTime = millis();

long btnDebounceTimer = 0;
boolean btnState = HIGH;

long buzzerTimer = 0;

long seg7FlashTimer = millis();
byte seg7Position = 0;
int score = 0;

/*
   按鈕狀態偵測，包含除彈跳。
   如果狀態有變化回傳true。
*/
boolean scanBtnState() {
  // 讀取Pin腳電位狀態
  boolean tempBtnState = digitalRead(BTN_PIN);
  // 判定電位有無變化
  if (btnState != tempBtnState) {
    // 判定是否已在計時
    if (btnDebounceTimer > 0) {
      // 判定已超過彈跳時間
      if (nowTime - btnDebounceTimer > DEBONUCE_PERIOD) {
        // 確定改變按鈕狀態
        btnState = tempBtnState;
        return true;
      }
    } else {
      // 紀錄電位變化時的時刻(啟動計時)
      btnDebounceTimer = nowTime;
    }
  } else {
    // 清空時刻
    btnDebounceTimer = 0;
  }
  return false;
}

/*
  七段顯示器工具: 設定數字在指定位置
*/
void setNumber(byte pos, byte number) {
  // 確認位置
  for (byte i = 0; i < 3; i++) {
    digitalWrite(SEG7_POSITION_PIN[i], POSITION[pos][i]);
  }
  // 寫入數字
  for (byte i = 0; i < 7; i++) {
    digitalWrite(SEG7_LED_PIN[i], NUMBER[number][i]);
  }
}

/*
  七段顯示器工具: 回傳該位置的數值，負數代表空白。
*/
int getNumber(byte pos, int value) {
  switch (pos) {
    case 0:
      if (value < 0) return -1;
      return (value % 10) / 1;
    case 1:
      if (value < 10) return -1;
      return (value % 100) / 10;
    case 2:
      if (value < 100) return -1;
      return (value % 1000) / 100;
  }
}

void setup() {
  // 初始化
  //  Serial.begin(9600);
  pinMode(BTN_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);

  // 七段顯示器初始化
  for (byte i = 0; i < 7; i++) {
    pinMode(SEG7_LED_PIN[i], OUTPUT);
  }
  for (byte i = 0; i < 3; i++) {
    pinMode(SEG7_POSITION_PIN[i], OUTPUT);
  }
}

void loop() {
  nowTime = millis();

  /*
    Display
  */
  if (nowTime - seg7FlashTimer > SEG7_FLASH_PERIOD) {
    int number = getNumber(seg7Position, score);
    if (number >= 0)
      setNumber(seg7Position, number);

    // 遞增顯示位置
    if (++seg7Position > 2) seg7Position = 0;
    // 更新timer
    seg7FlashTimer = nowTime;
  }

  /*
    Game Over
  */
  if (nowTime > GAMEOVER_PERIOD) {
    // 蜂鳴器急促長音
    if (nowTime - buzzerTimer > 700) {
      tone(BUZZER_PIN, 1250, 600);
      buzzerTimer = nowTime;
    }
    return;
  }

  /*
     Score
  */
  if (scanBtnState()) {
    // 按鈕 Click
    if (btnState == HIGH) {
      // 蜂鳴器短音
      tone(BUZZER_PIN, 1250, 100);
      // 加分
      score += ADD_SCORE;
      if (score > MAX_SCORE) score = MAX_SCORE;
    }
  }

}
