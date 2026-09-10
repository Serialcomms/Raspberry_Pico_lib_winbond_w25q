#ifndef SPIFFS_PICO_WINBOND_READ_H
#define SPIFFS_PICO_WINBOND_READ_H

#define DMA_TRANSFER_COUNT_MASK 0x0fffff

extern uint8_t command_buffer[12] __attribute__((aligned(4))); 

int winbond_read_data_spi(pico_spi_device_t *pico_spi, uint32_t read_address, uint8_t *read_buffer, size_t read_length);
uint32_t winbond_read_data_dma(pico_spi_device_t *pico_spi, uint32_t read_address, uint8_t *read_buffer, size_t read_length);

#endif