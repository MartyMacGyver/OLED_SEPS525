# Driving OLEDs based on the SEPS525 chipset

Another display in my collection is a [Newhaven NHD-1.69-160128UGC3 RGB OLED display](http://www.newhavendisplay.com/nhd169160128ugc3-p-5603.html). I'm basing this work on (my SSD1322 project](https://github.com/MartyMacGyver/OLED_SSD1322).

4-Wire SPI (software and 8-bit HW) works, but non-8-bit SPI in hardware is not possible on the Mega and this chipset doesn't support 3-Wire SPI. 16 bit color works but 18 bit doesn't; HW SPI is incredibly noisy, yet a finger on pins 13,15,17 and 19 removes virtually all the noise. Odd.

This *should* work for various SEPS525 displays (e.g., others in the Newhaven series using this chipset).

Setup: I used an Arduino Mega 2560 with the latest Arduino 1.5.8 IDE. I used a 74LVC245 for voltage translaton. The display was wired per the serial specs for it, with the level shifted Arduino pins going to the corresponding board pins. The reference voltage for the '245 and the display was 3.3V. (Never apply 5V directly to this display!)

**Further reading:**

Specifications: http://www.newhavendisplay.com/specs/NHD-1.69-160128UGC3.pdf

Chipset app notes: http://www.newhavendisplay.com/app_notes/SEPS525.pdf
