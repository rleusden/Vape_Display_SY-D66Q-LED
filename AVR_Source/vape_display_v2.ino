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
 
#include "ConfigDisplay.h"
#include "CharliePlex.h"
#include "DisplayTimer_AVR.h"

void setup() {
  Serial.begin(115200);

  CharliePlex::begin();

#ifdef __AVR__
  DisplayTimerAVR::begin(CharliePlex::scanStep);
#else
  #error "No timer implementation for this board yet."
#endif
}

void loop() {
  // Demo: update framebuffer at low rate; refresh runs in ISR
  static uint8_t p = 0;
  static bool up = true;
  static uint32_t t0 = 0;

  if (millis() - t0 > 250) {
    t0 = millis();

    CharliePlex::clear();
    CharliePlex::showPercent(p, true);
    CharliePlex::setCharging((p % 2) == 0);
    CharliePlex::setLevelBars(p);

    bool flip = ((millis() / 500) % 2) == 0;
    CharliePlex::setDrop(flip, !flip, flip, !flip);

    if (up) { if (p >= 100) up = false; else p += 5; }
    else    { if (p == 0)   up = true;  else p -= 5; }
  }
}
