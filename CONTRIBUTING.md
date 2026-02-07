# Contributing

Contributions are very welcome! The goal of this project is to make discarded SY-D66Q-LED vape displays reusable for open hardware projects.

## What you can contribute

* Alternative pin mappings for different hardware revisions
* Bug fixes or performance improvements
* New UI features (animations, icons, charging modes)
* Examples for other platforms (ESP32, RP2040, etc.)
* Documentation improvements and translations

## Mapping a new display

If your display revision uses different AB codes:

1. Use the scan/mapping sketch to determine all LED pairs
2. Create a new mapping in `ConfigDisplay.h`
3. Document the revision markings on the PCB
4. Add photos if possible

## Pull request guidelines

* Keep changes focused and small
* Follow the existing code style
* Update documentation when behavior changes
* Test on real hardware if possible

## License

This project is licensed under **GPL-3.0**.
By submitting code you agree that your contributions will be released under the same license.
