#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "hardware/spi.h"
#include "pico/status_led.h"

#include "winbond_W25Q_main.h"

#define PICO_PIN_MISO 16
#define PICO_PIN_CS   17
#define PICO_PIN_SCK  18
#define PICO_PIN_MOSI 19
#define PICO_SPI_BAUD (40 * 1000 * 1000)
#define PICO_SPI_DEVICE spi0

pico_spi_device_t winbond_spi_struct = {

    .PIN_CS = PICO_PIN_CS,
    .PIN_SCK = PICO_PIN_SCK,
    .PIN_MOSI = PICO_PIN_MOSI,
    .PIN_MISO = PICO_PIN_MISO,
    .SPI_BAUD = PICO_SPI_BAUD,
    .device = PICO_SPI_DEVICE,
    .DMA_BYTES = 256,
    .dma_channel_tx = 0,
    .dma_channel_rx = 0,
    .dma_channel_mask_tx_rx = 0

};

pico_spi_device_t *winbond_spi = &winbond_spi_struct;

void shell_clear_screen();

void shell_query_winbond();

void shell_flash_test();

void shell_reboot_pico(uint32_t reboot_delay_ms);

int main() {

  stdio_uart_init();

  shell_clear_screen();

  status_led_init();

  lib_winbond_init_all(winbond_spi);

  shell_query_winbond();

  shell_flash_test();
    
  shell_reboot_pico(1000);

}

void shell_query_winbond() {

  uint8_t winbond_query[8];

  memset(winbond_query, 0, 8);

  lib_winbond_query_device_id(winbond_spi, winbond_query);

  printf("\r\n");

  printf("===========================================\r\n");

  printf("Winbond Device ID = %02x-%02x\r\n", 
    winbond_query[0],
    winbond_query[1]); 

  memset(winbond_query, 0, 8);

  lib_winbond_query_jedec_id(winbond_spi, winbond_query);

    printf("Winbond JEDEC  ID = %02x-%02x-%02x\r\n", 
    winbond_query[0],
    winbond_query[1],
    winbond_query[2]
    );
  
  memset(winbond_query, 0, 8);

  lib_winbond_query_unique_id(winbond_spi, winbond_query);

  printf("Winbond Unique ID = %02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x\r\n", 
    winbond_query[0],
    winbond_query[1],
    winbond_query[2],
    winbond_query[3],
    winbond_query[4], 
    winbond_query[5],
    winbond_query[6],
    winbond_query[7]
  
  );

  printf("===========================================\r\n");

  printf("\r\n");

}

void shell_flash_test() {

    uint8_t flash_random_data_write[256];
    uint8_t flash_random_data_read[256];

    uint i = 0;

    memset(flash_random_data_write, 0xff, 256);
    memset(flash_random_data_read, 0x00, 256);

    printf("Generating random flash data, bytes = 256\r\n");

    do {

      flash_random_data_write[i] = (uint8_t) (rand() >> 16);


    } while (++i < 256);
       
    printf("Erasing external flash, 4k sector, address = 0\r\n");

    printf("\r\n");

    lib_winbond_erase_4k_sector(winbond_spi, 0);    

    lib_winbond_program_full_page(winbond_spi, 0, flash_random_data_write);

    lib_winbond_read_data_dma(winbond_spi, 0, flash_random_data_read, 256);

    bool flash_fail = false;

    size_t flash_index = 0;

    do {

        flash_fail = (flash_random_data_read[flash_index] != flash_random_data_write[flash_index]);

        flash_index += 1;

    } while (flash_index < 256);

    if (flash_fail) {

        printf("Winbond External Flash Test (DMA Read Method) = FAIL \r\n");

    } else {

        printf("Winbond External Flash Test (DMA Read Method) = PASS \r\n");

    }

    flash_index = 0;
    
    memset(flash_random_data_read, 0x00, 256);

    lib_winbond_read_data_spi(winbond_spi, 0, flash_random_data_read, 256);

    do {

        flash_fail = (flash_random_data_read[flash_index] != flash_random_data_write[flash_index]);

        flash_index += 1;

    } while (flash_index < 256);

    if (flash_fail) {

        printf("Winbond External Flash Test (SPI Read Method) = FAIL \r\n");

    } else {

        printf("Winbond External Flash Test (SPI Read Method) = PASS \r\n");

    }
}

void shell_reboot_pico(uint32_t reboot_delay_ms) {

  printf("\r\n");

  printf("Rebooting Pico to boot select mode in %i ms ...\r\n", reboot_delay_ms);

  absolute_time_t reboot_time = make_timeout_time_ms(reboot_delay_ms);

  do {

    busy_wait_ms(50);

    bool status_led_state = status_led_get_state();

    status_led_set_state(!status_led_state);

  } while (time_reached(reboot_time) == false);

  status_led_set_state(false);

  rom_reset_usb_boot(0, 0);

}

void shell_clear_screen() {

  printf("\033[H");    // cursor home
  printf("\033[2J");   // clear screen
  printf("\033[3J");   // clear scrollback
  printf("\033[40m");  // black background
  printf("\033[12I");  // set local terminal echo on
  printf("\033[?25l"); // hide cursor (reduce flicker)

}
