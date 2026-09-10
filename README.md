# Raspberry Pico lib Winbond w25q (September 2026 update)

Raspberry Pico device driver library for Winbond W25Q flash memory devices using the Pico's DMA and SPI peripherals.

Tested with :

* Raspberry Pico SDK v2.3.1
* Raspberry Pico RP2350 board
* SPIFFS flash file system
* Adafruit W25Q64JV Flash Breakout https://www.adafruit.com/product/5636

This library provides a simple, safe, high‑level API for interacting with Winbond W25Q‑series SPI flash chips (W25Q32, W25Q64, W25Q128, etc.) using the Raspberry Pi Pico and the official Pico SDK.

It abstracts away all the low‑level details:

* page‑aligned writes

* busy polling

* write‑enable latch

* JEDEC ID handling

* sector/block erase (4k, 32k, 64k)

* status register quirks

  


Acknowledgements:

[ajtaji](https://puremetalforge.ajtaji.com/) for identifying the following incorrect behaviours :

a. Part-page programming can cross a 256-byte flash page boundary.

b. Full-chip erase reports success unconditionally.

c. Busy-wait checks with no timeout could cause program hang.

d. Write-Enable condition set and not checked.




