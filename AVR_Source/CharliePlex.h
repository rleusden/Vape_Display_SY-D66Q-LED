/*
 * SY-D66Q-LED Vape Display Driver
 * Copyright (C) 2026 rleusden
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
#pragma once
#include <Arduino.h>
#include "ConfigDisplay.h"

/*
  CharliePlex.h – header only driver for SY-D66Q-LED
  LOW → HIGH convention
*/

namespace CharliePlex {

// ---------- Core state ----------

static volatile uint8_t fb[30];   // 6*(6-1)
static uint8_t scanIdx = 0;

// ---------- Basic helpers ----------

inline uint8_t pinCount() { return sizeof(DISPLAY_PINS); }
inline uint8_t ledCount() { return pinCount() * (pinCount() - 1); }

inline void allHiZ() {
  for (uint8_t i = 0; i < pinCount(); i++)
    pinMode(DISPLAY_PINS[i], INPUT);
}

inline void clear() {
  for (uint8_t i = 0; i < ledCount(); i++) fb[i] = 0;
}

// AB → index
inline int pairToIndex(uint8_t low, uint8_t high) {
  if (low >= pinCount() || high >= pinCount()) return -1;
  if (low == high) return -1;

  uint8_t hc = high;
  if (high > low) hc--;

  return (int)(low * (pinCount() - 1) + hc);
}

// ---------- Fast IO driver ----------

inline void turnOnPairFast(uint8_t lowIdx, uint8_t highIdx) {
  uint8_t lowPin  = DISPLAY_PINS[lowIdx];
  uint8_t highPin = DISPLAY_PINS[highIdx];

  // everything Hi-Z
  for (uint8_t i = 0; i < pinCount(); i++) {
    uint8_t p = DISPLAY_PINS[i];
    *portModeRegister(digitalPinToPort(p)) &= ~digitalPinToBitMask(p);
  }

  // LOW
  *portModeRegister(digitalPinToPort(lowPin)) |= digitalPinToBitMask(lowPin);
  *portOutputRegister(digitalPinToPort(lowPin)) &= ~digitalPinToBitMask(lowPin);

  // HIGH
  *portModeRegister(digitalPinToPort(highPin)) |= digitalPinToBitMask(highPin);
  *portOutputRegister(digitalPinToPort(highPin)) |= digitalPinToBitMask(highPin);
}

inline void setAB(uint8_t low, uint8_t high, bool on) {
  int idx = pairToIndex(low, high);
  if (idx >= 0 && idx < (int)ledCount())
    fb[idx] = on ? 1 : 0;
}

inline void setAB_2digit(uint8_t ab, bool on) {
  setAB(abLow(ab), abHigh(ab), on);
}

// ---------- Refresh (called from timer ISR) ----------

inline void scanStep() {
  const uint8_t n = ledCount();

  for (uint8_t tries = 0; tries < n; tries++) {
    scanIdx++;
    if (scanIdx >= n) scanIdx = 0;

    if (fb[scanIdx]) {
      uint8_t low  = scanIdx / (pinCount() - 1);
      uint8_t high = scanIdx % (pinCount() - 1);
      if (high >= low) high++;

      turnOnPairFast(low, high);
      return;
    }
  }
  allHiZ();
}

// ---------- UI helpers ----------

inline void begin() {
  allHiZ();
  clear();
}

// ----- 7 segment digits -----

inline void drawDigitLeft(uint8_t d) {
  static const bool m[10][7] = {
    {1,1,1,1,1,1,0}, {0,1,1,0,0,0,0}, {1,1,0,1,1,0,1},
    {1,1,1,1,0,0,1}, {0,1,1,0,0,1,1}, {1,0,1,1,0,1,1},
    {1,0,1,1,1,1,1}, {1,1,1,0,0,0,0}, {1,1,1,1,1,1,1},
    {1,1,1,1,0,1,1}
  };
  if (d > 9) d = 9;

  setAB_2digit(LA, m[d][0]);
  setAB_2digit(LB, m[d][1]);
  setAB_2digit(LC, m[d][2]);
  setAB_2digit(LD, m[d][3]);
  setAB_2digit(LE, m[d][4]);
  setAB_2digit(LF, m[d][5]);
  setAB_2digit(LG, m[d][6]);
}

inline void drawDigitRight(uint8_t d) {
  static const bool m[10][7] = {
    {1,1,1,1,1,1,0}, {0,1,1,0,0,0,0}, {1,1,0,1,1,0,1},
    {1,1,1,1,0,0,1}, {0,1,1,0,0,1,1}, {1,0,1,1,0,1,1},
    {1,0,1,1,1,1,1}, {1,1,1,0,0,0,0}, {1,1,1,1,1,1,1},
    {1,1,1,1,0,1,1}
  };
  if (d > 9) d = 9;

  setAB_2digit(RA, m[d][0]);
  setAB_2digit(RB, m[d][1]);
  setAB_2digit(RC, m[d][2]);
  setAB_2digit(RD, m[d][3]);
  setAB_2digit(RE, m[d][4]);
  setAB_2digit(RF, m[d][5]);
  setAB_2digit(RG, m[d][6]);
}

inline void showPercent(uint8_t p, bool showSign = true) {
  if (p > 99) p = 99;

  uint8_t tens = p / 10;
  uint8_t ones = p % 10;

  if (p < 10) {
    setAB_2digit(LA,false); setAB_2digit(LB,false);
    setAB_2digit(LC,false); setAB_2digit(LD,false);
    setAB_2digit(LE,false); setAB_2digit(LF,false);
    setAB_2digit(LG,false);
  } else {
    drawDigitLeft(tens);
  }

  drawDigitRight(ones);
  setAB_2digit(ICON_PERCENT, showSign);
}

inline void setCharging(bool on) {
  setAB_2digit(ICON_THUNDER, on);
}

inline void setDrop(bool rb, bool ro, bool lb, bool lo) {
  setAB_2digit(DROP_RB, rb);
  setAB_2digit(DROP_RO, ro);
  setAB_2digit(DROP_LB, lb);
  setAB_2digit(DROP_LO, lo);
}

// ----- Level bars with blink <20% -----

inline void setLevelBars(uint8_t percent) {
  uint8_t level = percentToLevel4(percent);

  bool blink = (percent < 20);
  bool blinkOn = ((millis() / 300) % 2) == 0;

  for (uint8_t i = 0; i < 4; i++) {
    bool on = (i < level);

    if (blink && i == 0)
      on = blinkOn;

    setAB_2digit(LEVELS[i], on);
  }
}

} // namespace
