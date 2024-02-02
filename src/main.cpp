#include <Arduino.h>
#include "dice.h"
#include "TFT_eSPI.h"

#define UP_BTN 0
#define DOWN_BTN 14

// 最大延迟时时间。值越大，摇动时间越长
#define MAX_DELAY 800

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);
// 当前骰子面值
byte curr_dice = 0;
// 是否在标题界面
bool in_splash = false;

unsigned long prevMillis = 0;
unsigned long currMillis = 0;

// 展示标题界面
void _splashScreen()
{
  if (in_splash)
    return;
  tft.fillScreen(TFT_BLACK);

  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(4);
  tft.drawString("Dice", TFT_HEIGHT / 2, TFT_WIDTH / 2 - 30);
  tft.drawString("Roller", TFT_HEIGHT / 2, TFT_WIDTH / 2 + 20);

  tft.setTextDatum(BR_DATUM);
  tft.setTextSize(2);
  tft.drawString("By Xster", TFT_HEIGHT - 10, TFT_WIDTH - 10);

  in_splash = true;
}

// 开始摇骰子
void _draw()
{
  in_splash = false;
  u_int32_t curr_delay = random(5);
  u_int32_t cycles = 1;
  byte final_dice = 0;

  sprite.setTextSize(3);

  while (curr_delay < MAX_DELAY)
  {
    if (digitalRead(UP_BTN) == 0)
    {
      _splashScreen();
      return;
    }

    currMillis = millis();
    if (currMillis - prevMillis > curr_delay)
    {
      prevMillis = currMillis;

      sprite.fillSprite(TFT_BLACK); // Clear screen
      sprite.pushImage(0, 0, 128, 128, dice[curr_dice]);

      sprite.drawString("Rolling...", 0, 130);
      sprite.pushSprite(0, 0);

      curr_delay = random(curr_delay, curr_delay + cycles * cycles);
      delay(curr_delay);

      Serial.println(String(cycles) + " - " + String(curr_delay) + " - " + String(curr_dice));
      cycles++;

      if (curr_delay >= MAX_DELAY)
      {
        final_dice = curr_dice;
        break;
      }

      curr_dice = random(0, 6);
    }
  }

  Serial.println("Done: " + String(final_dice + 1));

  sprite.setTextSize(3);
  sprite.drawString("Done: " + String(final_dice + 1) + "       ", 0, 130);
  sprite.pushSprite(0, 0);
}

void setup()
{
  Serial.begin(115200);
  randomSeed(analogRead(0));

  pinMode(UP_BTN, INPUT_PULLUP);
  pinMode(DOWN_BTN, INPUT_PULLUP);

  tft.init();
  tft.setRotation(3);

  sprite.createSprite(320, 170);
  sprite.setSwapBytes(1);

  tft.fillScreen(TFT_BLACK);

  _splashScreen();
}

void loop()
{
  // 开始摇骰子
  if (digitalRead(DOWN_BTN) == 0)
  {
    _draw();
  }

  // 返回标题界面
  if (digitalRead(UP_BTN) == 0)
  {
    _splashScreen();
  }

  delay(50);
}