/*
  Simple SY-D66Q-LED Mapper
  ---------------------------------------
  Enter AB codes via the serial monitor.
  How to use:
  - Upload sketch
  - Open Serial Monitor @ 115200
  - Typ: 43<enter> LED for AB=43 turns on
 
  IMPORTANT:
  Values like 11, 22, 33, 44, 55, 66 are NOT valid
  because a charlieplex LED always requires:
    - one pin LOW
    - a DIFFERENT pin HIGH
*/

#include <Arduino.h>

// ---- Adjust to your wiring ----
const uint8_t PINS[] = {2, 3, 4, 5, 6, 7};
inline uint8_t pinCount() { return sizeof(PINS); }

// Set all pins Hi-Z
void allHiZ() {
  for (uint8_t i = 0; i < pinCount(); i++) {
    pinMode(PINS[i], INPUT);
  }
}

// Drive AB pair: A=LOW, B=HIGH
void driveAB(uint8_t low, uint8_t high) {
  if (low >= pinCount() || high >= pinCount()) return;
  if (low == high) return;        // invalid combination

  for (uint8_t i = 0; i < pinCount(); i++) {
    if (i == low) {
      pinMode(PINS[i], OUTPUT);
      digitalWrite(PINS[i], LOW);
    }
    else if (i == high) {
      pinMode(PINS[i], OUTPUT);
      digitalWrite(PINS[i], HIGH);
    }
    else {
      pinMode(PINS[i], INPUT);    // Hi-Z
    }
  }
}

// Read one line from Serial
String readLine() {
  String s;
  while (true) {
    if (Serial.available()) {
      char c = Serial.read();

      if (c == '\r') continue;
      if (c == '\n') {
        s.trim();
        return s;
      }
      s += c;
    }
  }
}

void showHelp() {
  Serial.println();
  Serial.println("Enter AB code (e.g. 43 -> pin4 LOW, pin3 HIGH)");
  Serial.println("Type 'off' to disable all LEDs");
  Serial.println("Invalid examples: 11,22,33,44,55,66 (same pins)");
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  allHiZ();

  Serial.println("SY-D66Q-LED SIMPLE MAPPER");
  Serial.print("Using ");
  Serial.print(pinCount());
  Serial.println(" pins");
  showHelp();
}

void loop() {
  String cmd = readLine();

  if (cmd.equalsIgnoreCase("off")) {
    allHiZ();
    Serial.println("All off");
    return;
  }

  // Expect exactly two digits
  if (cmd.length() == 2 && isDigit(cmd[0]) && isDigit(cmd[1])) {
    uint8_t low  = cmd[0] - '0';
    uint8_t high = cmd[1] - '0';

    if (low == high) {
      Serial.println("ERROR: A and B must be different pins!");
      return;
    }

    driveAB(low, high);

    Serial.print("AB=");
    Serial.print(low);
    Serial.print(high);
    Serial.println(" activated");
    return;
  }

  Serial.println("Unknown command – enter two digits like 43");
}
