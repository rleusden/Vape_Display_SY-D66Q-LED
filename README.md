# SY-D66Q-LED Display Driver (Arduino)

Give discarded vape displays a second life 💚

This project provides an easy-to-use Arduino driver for the **SY-D66Q-LED** display module found in many disposable vape devices. Instead of throwing these tiny displays away after only weeks of use, you can reuse them in your own electronics projects such as battery monitors, power banks, chargers, or DIY gadgets.

The driver is **header-only**, uses **fast I/O**, and supports **timer-based refresh** for smooth, flicker-free output.

---

## ✨ Features

* Works with the SY-D66Q-LED charlieplex display
* Only **6 GPIO pins** required
* Fast direct port I/O for stable brightness
* Optional **timer interrupt refresh (AVR)**
* Built-in UI helpers:

  * Two 7-segment digits (0–99%)
  * Percent icon
  * Charging/Thunder icon
  * 4-step battery level indicator
  * Low-battery warning: **first bar blinks below 20%**
  * Drop symbol

### Clean structure

* **ConfigDisplay.h** – your pin & segment mapping
* **CharliePlex.h** – display driver (header-only)
* **DisplayTimer_AVR.h** – timer refresh for Arduino Nano/Uno
* **vape_display.ino** – demo sketch
* **hardware.md** – photos & wiring

---

## 🛠 Hardware

Each LED is controlled using **charlieplexing**:

* one pin driven **LOW**
* one pin driven **HIGH**
* all others **Hi-Z (input)**

Example AB notation:
`43 → pin 4 LOW, pin 3 HIGH`

The order in `DISPLAY_PINS[]` must match your wiring.

🛠 Full hardware description, photos, and wiring details:
🧰 **[hardware.md](hardware.md)**

---

## 🚀 Quick Start

1. Connect the display to your Arduino (use series resistors!)
2. Adjust pin order in `ConfigDisplay.h` if needed
3. Upload the demo sketch

### Minimal example

```cpp
#include "ConfigDisplay.h"
#include "CharliePlex.h"
#include "DisplayTimer_AVR.h"

void setup() {
  Serial.begin(115200);

  CharliePlex::begin();

#ifdef __AVR__
  DisplayTimerAVR::begin(CharliePlex::scanStep);
#endif
}

void loop() {
  // Refresh runs in timer ISR – update UI here
  CharliePlex::clear();
  CharliePlex::showPercent(73);
  CharliePlex::setCharging(true);
  CharliePlex::setLevelBars(73);
}
```

That’s it – the timer handles all multiplexing for you.

---

## 🧩 What can you build?

* Battery gauge using LTC2944 / INA219
* Power bank user interface
* Charger status display
* Display for temperature and humidity sensors
* Charlieplex projects for similar displays

---

## ⚠️ Safety Notes

* Always use **series resistors** on each display line
* Never drive multiple LEDs directly without the driver
* Verify your pin mapping before connecting power

---

## 🤝 Contributing

Different display batches may use different AB mappings.
Contributions are welcome:

* New mappings
* Platform timers (ESP32 / RP2040)
* UI animations
* Documentation improvements

See **[CONTRIBUTING.md](CONTRIBUTING.md)** for details.

---

## 📜 License

Released under the **GNU GPL v3.0**.
See **[LICENSE](LICENSE)** for the full text.

---

## 🌱 Why this project?

Disposable electronics create huge amounts of e-waste.
These displays are often thrown away while still perfectly functional.
This project aims to help makers **reuse instead of discard**.

Happy hacking! ✨
