# Raspberry Pico lib Winbond w25q

Raspberry Pico device driver library for Winbond W25Q flash memory devices using the Pico's DMA and SPI peripheral.

Tested with :

* Raspberry Pico RP2350
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

