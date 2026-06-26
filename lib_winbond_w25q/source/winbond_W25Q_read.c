#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/dma.h"
#include "hardware/regs/spi.h"
#include "winbond_W25Q_core.h"
#include "winbond_W25Q_main.h"
#include "winbond_W25Q_read.h"
#include "winbond_W25Q_query.h"

static uint32_t winbond_get_dma_bytes_read(pico_spi_device_t *pico_spi, uint32_t read_length);
static uint32_t winbond_start_read_data_dma(pico_spi_device_t *pico_spi, uint8_t *read_buffer, size_t read_length);

uint32_t winbond_read_data_spi(pico_spi_device_t *pico_spi, uint32_t read_address, uint8_t *read_buffer, size_t read_length) {
    
    winbond_set_command_and_address(READ_DATA, read_address);

    int spi_read_bytes = 0;

    if (spi_is_writable(pico_spi->device)) {

        winbond_chip_select(pico_spi);

        int spi_write_bytes = spi_write_blocking(pico_spi->device, command_buffer, 4);

        if (spi_write_bytes) {

            spi_read_bytes = spi_read_blocking(pico_spi->device, repeat_data_ff, read_buffer, read_length); 

        }

        winbond_chip_release(pico_spi);

    }
    
    return (uint32_t) spi_read_bytes;
}

uint32_t winbond_read_data_dma(pico_spi_device_t *pico_spi, uint32_t read_address, uint8_t *read_buffer, size_t read_length) {

    uint32_t read_dma_bytes = 0;
    
    winbond_set_command_and_address(READ_DATA, read_address);

    if (spi_is_writable(pico_spi->device)) {

        winbond_chip_select(pico_spi);

        int spi_write_bytes = spi_write_blocking(pico_spi->device, command_buffer, 4);

        if (spi_write_bytes) {

            read_dma_bytes = winbond_start_read_data_dma(pico_spi, read_buffer, read_length);

        }

        winbond_chip_release(pico_spi);

    }

    return read_dma_bytes;
}

static uint32_t winbond_start_read_data_dma(pico_spi_device_t *pico_spi, uint8_t *read_buffer, size_t read_length) {

    winbond_set_dma_transfer_count_tx_rx(pico_spi, read_length);
          
    dma_channel_set_write_addr(pico_spi->dma_channel_rx, read_buffer, false);

    dma_start_channel_mask(pico_spi->dma_channel_mask_tx_rx); // starts both DMA channels
    
    dma_channel_wait_for_finish_blocking(pico_spi->dma_channel_rx);

    return winbond_get_dma_bytes_read(pico_spi, read_length);

}

static uint32_t winbond_get_dma_bytes_read(pico_spi_device_t *pico_spi, uint32_t read_length) {

    uint32_t transfer_count = dma_hw->ch[pico_spi->dma_channel_rx].transfer_count;

    return read_length - (transfer_count & DMA_TRANSFER_COUNT_MASK);

}

