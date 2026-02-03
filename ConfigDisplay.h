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

/*
  ConfigDisplay.h
  ----------------
  Configuration for the SY-D66Q-LED vape display (charlieplex, LOW->HIGH convention)

  Convention:
    AB means: pin A driven LOW, pin B driven HIGH, all others Hi-Z.
    Example: 43 => low=4, high=3.
*/

// ----------------------
// Charlieplex pins (change to match your wiring order)
// ----------------------
static const uint8_t DISPLAY_PINS[] = {2, 3, 4, 5, 6, 7}; // 6 pins used for charlieplex

// Refresh timing
static const uint16_t DISPLAY_REFRESH_US = 800;  // tune for flicker/brightness

// ----------------------
// Segment / icon mapping (your measured AB codes)
// ----------------------

// Left digit segments
#define LA  43
#define LB  34
#define LC  23
#define LD  24
#define LE  14
#define LF  13
#define LG  12

// Right digit segments
#define RA  54
#define RB  45
#define RC  53
#define RD  35
#define RE  52
#define RF  25
#define RG  15

// Icons
#define ICON_PERCENT 42
#define ICON_THUNDER 32

// Drop (color segments)
#define DROP_RB 10
#define DROP_RO 50
#define DROP_LB 30
#define DROP_LO 40

// Battery fill levels (empty->full): 25%..100%  (4 steps)
static const uint8_t LEVELS[] = {21, 31, 41, 51}; // 4 steps

// ----------------------
// Helpers for AB codes
// ----------------------
static inline uint8_t abLow(uint8_t ab)  { return ab / 10; }
static inline uint8_t abHigh(uint8_t ab) { return ab % 10; }

// Percent -> level bars (0..4)
// 0: none, 1..4 correspond to LEVELS[] entries.
static inline uint8_t percentToLevel4(uint8_t p) {
  if (p >= 100) return 4;
  if (p < 25)  return 0;
  if (p < 50)  return 1;  // 25..49
  if (p < 75)  return 2;  // 50..74
  if (p < 100) return 3;  // 75..99
  return 4;
}