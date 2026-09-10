#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"

#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "hardware/dma.h"
#include "hardware/sync.h"
#include "hardware/address_mapped.h"
#include "hardware/regs/spi.h"

#include "winbond_W25Q_core.h"
#include "winbond_W25Q_query.h"
#include "winbond_W25Q_main.h"

uint8_t command_buffer[12] __attribute__((aligned(4))); 

uint8_t FLASH_SECTOR_BUFFER[WINBOND_FLASH_4K_SECTOR_SIZE] __attribute__((aligned(4)));

void winbond_chip_select(pico_spi_device_t *pico_spi) { 

    winbond_wait_for_spi_idle(pico_spi);
 
    gpio_put(pico_spi->PIN_CS, W25Q_CHIP_SELECT_TRUE); 

    busy_wait_us(1);   
}   

void winbond_chip_release(pico_spi_device_t *pico_spi) { 

    winbond_wait_for_spi_idle(pico_spi);

    gpio_put(pico_spi->PIN_CS, W25Q_CHIP_SELECT_FALSE); 

    busy_wait_us(1);
}   

void winbond_set_command_and_address(uint8_t winbond_command, uint32_t address) {

    clear_command_buffer();

    command_buffer[0] = winbond_command;
    command_buffer[1] = (address >> 16) & 0xFF;
    command_buffer[2] = (address >> 8) & 0xFF;
    command_buffer[3] = (address >> 0) & 0xFF;
}

void winbond_wait_for_spi_idle(pico_spi_device_t *pico_spi) {

    while (spi_is_busy(pico_spi->device)) { 
        
        tight_loop_contents();
    }

    busy_wait_us(1);
}

bool winbond_wait_for_write_complete(pico_spi_device_t *pico_spi, uint32_t wait_milliseconds) {

    bool write_busy = 0;
    bool wait_timeout = false;

    absolute_time_t write_busy_timeout = make_timeout_time_ms(wait_milliseconds);
   
    do {

        busy_wait_us(500);

        write_busy = winbond_get_write_busy(pico_spi);
        
        wait_timeout = time_reached(write_busy_timeout);

    } while (wait_timeout == false && write_busy == true); 
    
    if (wait_timeout == true) {

        printf("ERROR: Winbond Flash wait for write complete timed out after %lu milli seconds\n", wait_milliseconds);

        return false;

    } else {

        return true;
    }
 
}

bool winbond_get_write_busy(pico_spi_device_t *pico_spi) {
 
   uint8_t status_register = winbond_query_status_register(pico_spi, 1);

   return status_register & W25Q_FLASH_STATUS_BUSY_MASK;
}

bool winbond_get_write_enabled(pico_spi_device_t *pico_spi) {

   uint8_t status_register = winbond_query_status_register(pico_spi, 1);

   return status_register & W25Q_FLASH_WRITE_ENABLE_MASK;
}

bool winbond_set_write_enabled(pico_spi_device_t *pico_spi, bool enabled) {

    uint8_t winbond_command = enabled ? WRITE_ENABLE : WRITE_DISABLE;

    winbond_chip_select(pico_spi);

    int spi_bytes = spi_write_blocking(pico_spi->device, &winbond_command, 1);

    winbond_chip_release(pico_spi); 

    return (bool) spi_bytes;
}

bool winbond_set_erase_enabled(pico_spi_device_t *pico_spi, bool enabled) {

    return winbond_set_write_enabled(pico_spi, enabled);
}

uint32_t winbond_set_dma_transfer_count_tx_rx(pico_spi_device_t *pico_spi, uint32_t dma_bytes_tx_rx) {

    uint32_t encoded_transfer_count = dma_encode_transfer_count(dma_bytes_tx_rx);

    dma_channel_set_transfer_count(pico_spi->dma_channel_tx, encoded_transfer_count, false);
    dma_channel_set_transfer_count(pico_spi->dma_channel_rx, encoded_transfer_count, false);

    return encoded_transfer_count;
}

void clear_command_buffer() {
    
    memset(command_buffer, 0, sizeof(command_buffer));
}
