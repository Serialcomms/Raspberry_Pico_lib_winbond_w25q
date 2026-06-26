#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"

#include "winbond_W25Q_query.h"
#include "winbond_W25Q_core.h"
#include "winbond_W25Q_main.h"

//extern uint8_t query_command_buffer[12] __attribute__((aligned(4))); 

extern uint8_t query_command_buffer[];

winbond_device_query_t winbond_device_query;
winbond_device_query_t *device_query = &winbond_device_query;

int winbond_query_unique_id(pico_spi_device_t *pico_spi, uint8_t *query_result) {

    winbond_device_query.command_byte = READ_UNIQUE_ID;
    winbond_device_query.command_length = 5;
    winbond_device_query.response_length = 8;
    winbond_device_query.repeat_data = 0x00;

    return winbond_query_device(pico_spi, query_result);

}

int winbond_query_device_id(pico_spi_device_t *pico_spi, uint8_t *query_result) {

    winbond_device_query.command_byte = READ_DEVICE_ID;
    winbond_device_query.command_length = 4;
    winbond_device_query.response_length = 2;
    winbond_device_query.repeat_data = 0x00;

    return winbond_query_device(pico_spi, query_result);

}

int winbond_query_jedec_id(pico_spi_device_t *pico_spi, uint8_t *query_result) {

    winbond_device_query.command_byte = READ_JEDEC_ID;
    winbond_device_query.command_length = 1;
    winbond_device_query.response_length = 3;
    winbond_device_query.repeat_data = 0x00;

    return winbond_query_device(pico_spi, query_result);

}

uint8_t winbond_query_status_register(pico_spi_device_t *pico_spi, uint8_t register_number) {

    uint8_t register_read_command = winbond_get_register_read_command(register_number);

    uint8_t query_buffer[2] = {register_read_command, 0};

    winbond_chip_select(pico_spi);

    spi_write_read_blocking(pico_spi->device, query_buffer, query_buffer, 2);
    
    winbond_chip_release(pico_spi);

    return query_buffer[1];

}

void winbond_reset_device(pico_spi_device_t *pico_spi) {

    winbond_wait_for_write_complete(pico_spi);
    
    winbond_device_query.command_byte = RESET_ENABLE;
    winbond_device_query.command_length = 1;
    winbond_device_query.response_length = 0;
    winbond_device_query.repeat_data = 0x00;

    winbond_query_device(pico_spi, NULL);

    winbond_device_query.command_byte = RESET_DEVICE;

    winbond_query_device(pico_spi, NULL);

    printf("Winbond Flash Memory Device Reset\n");

    busy_wait_ms(50);

}

uint8_t winbond_get_register_read_command(uint8_t register_number) {

    switch (register_number) {

        case 1: return READ_STATUS_1;
        case 2: return READ_STATUS_2;
        case 3: return READ_STATUS_3;

        default: return INVALID_REGISTER;

    };

}

uint8_t winbond_get_register_write_command(uint8_t register_number) {

    switch (register_number) {

        case 1: return WRITE_STATUS_1;
        case 2: return WRITE_STATUS_2;
        case 3: return WRITE_STATUS_3;

        default: return INVALID_REGISTER;

    };

}

int winbond_query_device(pico_spi_device_t *pico_spi, uint8_t *read_result) {

    int spi_read_bytes = 0;

    winbond_set_command_and_address(device_query->command_byte, 0);

     if (spi_is_writable(pico_spi->device)) {

        winbond_chip_select(pico_spi);
  
        int spi_write_bytes = spi_write_blocking(pico_spi->device, command_buffer, device_query->command_length);

        if (spi_write_bytes && device_query->response_length) {

            spi_read_bytes = spi_read_blocking(pico_spi->device, repeat_data_ff, read_result, device_query->response_length);

        }
   
        winbond_chip_release(pico_spi);

    } else {

        printf("ERROR: Winbond query device, SPI is Not Writable\n");

    }
   
    return spi_read_bytes;

}