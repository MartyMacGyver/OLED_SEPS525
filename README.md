# Driving OLEDs based on the SEPS525 chipset

Another display in my collection is a [Newhaven NHD-1.69-160128UGC3 RGB OLED display](http://www.newhavendisplay.com/nhd169160128ugc3-p-5603.html). I'm basing this work on (my SSD1322 project](https://github.com/MartyMacGyver/OLED_SSD1322).

## Work in progress

Progress - "It aten't dead!" (with apologies to Pratchett) - I implemented the reset code from the specs and it lit right up!

This device doesn't support 3-Wire as far as I can tell. 4-Wire (software) works, but 16-bit SPI in hardware is not possible on the Mega.

This *should* work for various SEPS525 displays (e.g., others in the Newhaven series using this chipset).

Setup: I used an Arduino Mega 2560 with the latest Arduino 1.5.8 IDE. I used a 74LVC245 for voltage translaton. The display was wired per the serial specs for it, with the level shifted Arduino pins going to the corresponding board pins. The reference voltage for the '245 and the display was 3.3V. (Never apply 5V directly to this display!)

## Work in progress

**Further reading:**

Specifications: http://www.newhavendisplay.com/specs/NHD-1.69-160128UGC3.pdf

Chipset app notes: http://www.newhavendisplay.com/app_notes/SEPS525.pdf
