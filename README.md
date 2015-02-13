# Driving OLEDs based on the SEPS525 chipset

Another display in my collection is a [Newhaven NHD-1.69-160128UGC3 RGB OLED display](http://www.newhavendisplay.com/nhd169160128ugc3-p-5603.html). I'm basing this work on (my SSD1322 project](https://github.com/MartyMacGyver/OLED_SSD1322).

4-Wire SPI (software and 8-bit HW) works, but non-8-bit SPI in hardware is not possible on the Mega and this chipset doesn't support 3-Wire SPI. 16 bit color works but 18 bit doesn't.

This *should* work for various SEPS525 displays (e.g., others in the Newhaven series using this chipset).

Setup: I used an Arduino Mega 2560 with the latest Arduino 1.5.8 IDE. I used a 74LVC245 for voltage translaton. The display was wired per the serial specs for it, with the level shifted Arduino pins going to the corresponding board pins. The reference voltage for the '245 and the display was 3.3V. (Never apply 5V directly to this display!)

Hardware note: For some reason the CS line on mine was very susceptible to noise... touching my finger to it and a few nearby pins seemed to make the problem go away - a tiny 10 pF cap between the CS line (after the level shifter) and ground cleared that problem up. (This didn't happen with my SSD1322 device.)

**Further reading:**

Specifications: http://www.newhavendisplay.com/specs/NHD-1.69-160128UGC3.pdf

Chipset app notes: http://www.newhavendisplay.com/app_notes/SEPS525.pdf
