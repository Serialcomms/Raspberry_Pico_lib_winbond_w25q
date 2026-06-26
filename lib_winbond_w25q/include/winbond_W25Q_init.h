
#ifndef SPIFFS_PICO_WINBOND_INIT_H
#define SPIFFS_PICO_WINBOND_INIT_H

#define SPI_BITS_8 8

#define W25Q_CHIP_SELECT_FALSE 1
#define W25Q_CHIP_SELECT_TRUE 0

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct PICO_SPI_DEVICE_PROFILE {

    uint8_t PIN_CS;
    uint8_t PIN_SCK;
    uint8_t PIN_MOSI;
    uint8_t PIN_MISO;
    uint32_t SPI_BAUD;
    uint32_t DMA_BYTES;
    spi_inst_t *device;
    uint dma_channel_tx;
    uint dma_channel_rx;
    uint32_t dma_channel_mask_tx_rx;
        
} pico_spi_device_t;

void winbond_init_all(pico_spi_device_t *pico_spi);

#endif

