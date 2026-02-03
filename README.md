# SY-D66Q-LED Vape Display Driver (Arduino)

This project provides an open-source Arduino driver for the **SY-D66Q-LED** display module commonly found in vape devices.  
The display is a **charlieplexed LED module** with two 7-segment digits, battery level indicators, icons, and a multicolor drop symbol.  
It can be reused as a compact UI for battery-powered projects such as power banks, chargers, or energy monitors (e.g., LTC2944 based).

## Goal: Give these discarded vape displays a second life in useful DIY electronics 💚

## Features

- 6-wire charlieplex interface (LOW → HIGH driving convention)
- Two 7-segment digits for percentage display  
- Battery level bar (4 steps: 25–100%)
- Charging icon (thunder)
- Percent icon
- Multicolor drop indicator
- **Low battery warning:** first battery bar blinks when SoC < 20%
- Clean separation between:
  - `ConfigDisplay.h` – hardware mapping  
  - `vape_display.ino` – driver and demo logic

## Hardware concept

Each LED is addressed by driving:
- one pin **LOW**
- one pin **HIGH**
- all others **Hi-Z (input)**

Example AB notation:  
`43` → pin 4 LOW, pin 3 HIGH.

The pin order in `DISPLAY_PINS[]` must match your wiring.

## Usage

1. Connect the 6 display pins to an Arduino (with series resistors recommended).
2. Adjust pin order in `ConfigDisplay.h` if needed.
3. Upload `vape_display.ino`.
4. Use the provided functions:

```cpp
showPercent(73);        // show 73%
setCharging(true);      // thunder icon
setLevelBars(percent);  // 4-step battery bar with low-battery blink
setDrop(true,false,true,false); // custom drop color
```
##  Typical applications

- Battery monitors (LTC2944, INA219, BMS systems)
- DIY power banks
- Charger interfaces
- Portable sensor nodes

## Safety

- Always use series resistors, 390 Ohm is a good starting point, on each display line.
- Do not drive multiple LEDs directly without charlieplex logic.
- Use a low current < 100mA power supply during testing

## Contributing

Mapping may differ between hardware revisions.
Feel free to submit your own ConfigDisplay.h variants or improvements.
