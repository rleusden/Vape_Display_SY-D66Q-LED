/*
 * SY-D66Q-LED Vape Display Driver
 * Copyright (C) 2025 rleusden
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU GPL v3.0 license
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#include <Arduino.h>
#include "ConfigDisplay.h"

// ======================
// Charlieplex driver (LOW->HIGH convention)
// ======================
inline uint8_t pinCount() { return sizeof(DISPLAY_PINS); }
inline uint8_t ledCount() { return pinCount() * (pinCount() - 1); } // 30 for 6 pins

bool fb[30]; // framebuffer

void allHiZ() {
  for (uint8_t i = 0; i < pinCount(); i++) pinMode(DISPLAY_PINS[i], INPUT);
}

void turnOnPair(uint8_t lowIdx, uint8_t highIdx) {
  if (lowIdx >= pinCount() || highIdx >= pinCount()) return;
  if (lowIdx == highIdx) return;

  for (uint8_t i = 0; i < pinCount(); i++) {
    if (i == lowIdx) {
      pinMode(DISPLAY_PINS[i], OUTPUT);
      digitalWrite(DISPLAY_PINS[i], LOW);
    } else if (i == highIdx) {
      pinMode(DISPLAY_PINS[i], OUTPUT);
      digitalWrite(DISPLAY_PINS[i], HIGH);
    } else {
      pinMode(DISPLAY_PINS[i], INPUT); // Hi-Z
    }
  }
}

int pairToIndex(uint8_t low, uint8_t high) {
  if (low >= pinCount() || high >= pinCount()) return -1;
  if (low == high) return -1;
  uint8_t hc = high;
  if (high > low) hc--;
  return (int)(low * (pinCount() - 1) + hc);
}

void setAB(uint8_t low, uint8_t high, bool on) {
  int idx = pairToIndex(low, high);
  if (idx >= 0 && idx < (int)ledCount()) fb[idx] = on;
}

void setAB_2digit(uint8_t ab, bool on) {
  setAB(abLow(ab), abHigh(ab), on);
}

void clearFB() {
  for (uint8_t i = 0; i < ledCount(); i++) fb[i] = false;
}

// ======================
// Refresh engine
// ======================
static uint8_t scanIdx = 0;
static uint32_t lastMicros = 0;

void scanStep() {
  const uint8_t n = ledCount();

  for (uint8_t tries = 0; tries < n; tries++) {
    scanIdx++;
    if (scanIdx >= n) scanIdx = 0;

    if (fb[scanIdx]) {
      uint8_t low  = scanIdx / (pinCount() - 1);
      uint8_t high = scanIdx % (pinCount() - 1);
      if (high >= low) high++;
      turnOnPair(low, high);
      return;
    }
  }
  allHiZ();
}

void serviceDisplay() {
  uint32_t now = micros();
  if ((uint32_t)(now - lastMicros) >= DISPLAY_REFRESH_US) {
    lastMicros = now;
    scanStep();
  }
}

// ======================
// UI primitives (digits/icons/levels/drop)
// ======================
void blankLeftDigit() {
  setAB_2digit(LA,false); setAB_2digit(LB,false); setAB_2digit(LC,false);
  setAB_2digit(LD,false); setAB_2digit(LE,false); setAB_2digit(LF,false);
  setAB_2digit(LG,false);
}

void blankRightDigit() {
  setAB_2digit(RA,false); setAB_2digit(RB,false); setAB_2digit(RC,false);
  setAB_2digit(RD,false); setAB_2digit(RE,false); setAB_2digit(RF,false);
  setAB_2digit(RG,false);
}

void drawLeftDigit(uint8_t digit) {
  static const bool map[10][7] = {
    {1,1,1,1,1,1,0}, //0
    {0,1,1,0,0,0,0}, //1
    {1,1,0,1,1,0,1}, //2
    {1,1,1,1,0,0,1}, //3
    {0,1,1,0,0,1,1}, //4
    {1,0,1,1,0,1,1}, //5
    {1,0,1,1,1,1,1}, //6
    {1,1,1,0,0,0,0}, //7
    {1,1,1,1,1,1,1}, //8
    {1,1,1,1,0,1,1}  //9
  };
  if (digit > 9) digit = 9;
  setAB_2digit(LA, map[digit][0]);
  setAB_2digit(LB, map[digit][1]);
  setAB_2digit(LC, map[digit][2]);
  setAB_2digit(LD, map[digit][3]);
  setAB_2digit(LE, map[digit][4]);
  setAB_2digit(LF, map[digit][5]);
  setAB_2digit(LG, map[digit][6]);
}

void drawRightDigit(uint8_t digit) {
  static const bool map[10][7] = {
    {1,1,1,1,1,1,0}, //0
    {0,1,1,0,0,0,0}, //1
    {1,1,0,1,1,0,1}, //2
    {1,1,1,1,0,0,1}, //3
    {0,1,1,0,0,1,1}, //4
    {1,0,1,1,0,1,1}, //5
    {1,0,1,1,1,1,1}, //6
    {1,1,1,0,0,0,0}, //7
    {1,1,1,1,1,1,1}, //8
    {1,1,1,1,0,1,1}  //9
  };
  if (digit > 9) digit = 9;
  setAB_2digit(RA, map[digit][0]);
  setAB_2digit(RB, map[digit][1]);
  setAB_2digit(RC, map[digit][2]);
  setAB_2digit(RD, map[digit][3]);
  setAB_2digit(RE, map[digit][4]);
  setAB_2digit(RF, map[digit][5]);
  setAB_2digit(RG, map[digit][6]);
}

// Show percent with optional % sign and optional blinking left digit (<20%)
void showPercent(uint8_t percent, bool showPercentSign=true, bool blinkLeftIfLow=true) {
  if (percent > 99) percent = 99;
  uint8_t tens = percent / 10;
  uint8_t ones = percent % 10;

  // Blink logic: left digit toggles at ~2Hz if percent < 20
  bool blinkOn = true;
  if (blinkLeftIfLow && percent < 20) {
    blinkOn = ((millis() / 250) % 2) == 0; // 250ms on/off
  }

  if (percent < 10) {
    // for <10: normally blank left digit; if blinking enabled, we "blink blank" (so no visual change)
    // If you want a warning indicator, you could blink the % sign instead.
    blankLeftDigit();
  } else {
    if (blinkOn) drawLeftDigit(tens);
    else blankLeftDigit();
  }

  drawRightDigit(ones);
  setAB_2digit(ICON_PERCENT, showPercentSign);
}

void setCharging(bool on) {
  setAB_2digit(ICON_THUNDER, on);
}

void setDrop(bool rb, bool ro, bool lb, bool lo) {
  setAB_2digit(DROP_RB, rb);
  setAB_2digit(DROP_RO, ro);
  setAB_2digit(DROP_LB, lb);
  setAB_2digit(DROP_LO, lo);
}

void setLevelBars(uint8_t percent) {
  uint8_t level = percentToLevel4(percent);

  bool blink = (percent < 20);
  bool blinkOn = ((millis() / 300) % 2) == 0;

  for (uint8_t i = 0; i < sizeof(LEVELS); i++) {
    bool on = false;

    if (i < level) {
      on = true;
    }

    // Bij <20%: eerste segment laten knipperen
    if (blink && i == 0) {
      on = blinkOn;
    }

    setAB_2digit(LEVELS[i], on);
  }
}

// ======================
// Demo
// ======================
void setup() {
  Serial.begin(115200);
  allHiZ();
  clearFB();

  Serial.println(F("SY-D66Q-LED vape display demo (GitHub friendly split)."));
  Serial.print(F("Pins=")); Serial.print(pinCount());
  Serial.print(F(" LEDs=")); Serial.println(ledCount());
}

void loop() {
  serviceDisplay();

  // Demo: percent up/down, charging blink, levels, drop pattern
  static uint8_t p = 0;
  static bool up = true;
  static uint32_t lastStep = 0;

  if (millis() - lastStep >= 250) {
    lastStep = millis();

    clearFB();

    // 1) Percent with low warning blink (<20%)
    showPercent(p, true, true);

    // 2) Charging icon: blink when "charging"
    bool charging = (p % 2) == 0;
    bool chargeBlink = ((millis() / 300) % 2) == 0;
    setCharging(charging && chargeBlink);

    // 3) Level bars (4 steps)
    setLevelBars(p);

    // 4) Drop: simple alternating colors
    bool flip = ((millis() / 500) % 2) == 0;
    setDrop(flip, !flip, flip, !flip);

    // update p
    if (up) {
      if (p >= 100) up = false;
      else p += 5;
    } else {
      if (p == 0) up = true;
      else p -= 5;
    }
  }
}