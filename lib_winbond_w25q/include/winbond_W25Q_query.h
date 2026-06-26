
#ifndef SPIFFS_PICO_WINBOND_QUERY_H
#define SPIFFS_PICO_WINBOND_QUERY_H

#include "winbond_W25Q_init.h"

typedef struct WINBOND_DEVICE_QUERY {

    uint8_t command_byte;
    uint8_t command_length;
    uint8_t response_length;
    uint8_t repeat_data;


} winbond_device_query_t;

typedef enum W25_QUERY_COMMANDS {

    INVALID_REGISTER = 0x00,

    READ_REGISTER_1 = 0x05,
    READ_REGISTER_2 = 0x35,
    READ_REGISTER_3 = 0x15,

    WRITE_STATUS_1 = 0x01,
    WRITE_STATUS_2 = 0x31,
    WRITE_STATUS_3 = 0x11,

    READ_DATA = 0x03,
    READ_SFDP =  0x5A,   // Serial Flash Discoverable Parameter
   
    READ_STATUS_1 = 0x05,
    READ_STATUS_2 = 0x35,
    READ_STATUS_3 = 0x15,

    READ_JEDEC_ID = 0x9F,
    READ_UNIQUE_ID = 0x4B,
    READ_DEVICE_ID = 0x90,
   
    PAGE_PROGRAM = 0x02,
    ERASE_4K_SECTOR = 0x20,
    ERASE_32K_BLOCK = 0x52,
    ERASE_64K_BLOCK = 0xD8,
    ERASE_FULL_CHIP = 0x60,
    
    WRITE_ENABLE = 0x06,
    WRITE_DISABLE = 0x04,
    
    RESET_ENABLE = 0x66,
    RESET_DEVICE = 0x99

} w25q_command_t;

void winbond_reset_device(pico_spi_device_t *pico_spi);

int winbond_query_device(pico_spi_device_t *pico_spi, uint8_t *query_result);
int winbond_query_unique_id(pico_spi_device_t *pico_spi, uint8_t *query_result);
int winbond_query_device_id(pico_spi_device_t *pico_spi, uint8_t *query_result);
int winbond_query_jedec_id(pico_spi_device_t *pico_spi, uint8_t *query_result);

uint8_t winbond_get_register_read_command(uint8_t register_number);
uint8_t winbond_query_status_register(pico_spi_device_t *pico_spi, uint8_t register_number);

#endif
