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

#ifdef __AVR__

namespace DisplayTimerAVR {
  typedef void (*Callback)();
  static volatile Callback cb = nullptr;

  inline void begin(Callback callback, uint16_t ocr1a = 1666) {
    cb = callback;

    cli();

    // Timer1 CTC
    TCCR1A = 0;
    TCCR1B = 0;
    TCCR1B |= (1 << WGM12);   // CTC
    TCCR1B |= (1 << CS11);    // prescaler 8

    OCR1A = ocr1a;            // ~1200 Hz at 16MHz/8
    TIMSK1 |= (1 << OCIE1A);  // enable compare interrupt

    sei();
  }

  inline void end() {
    cli();
    TIMSK1 &= ~(1 << OCIE1A);
    sei();
    cb = nullptr;
  }
}

ISR(TIMER1_COMPA_vect) {
  if (DisplayTimerAVR::cb) DisplayTimerAVR::cb();
}

#endif
