
#include "pico/stdio.h"
#include "pico/stdlib.h"
#include "pico/printf.h"
#include "hardware/spi.h"
#include "hardware/dma.h"
#include "hardware/gpio.h"
#include "hardware/regs/spi.h"
#include "hardware/structs/spi.h"
#include "pico/binary_info.h"

#include "winbond_W25Q_init.h"

static void winbond_init_gpio(pico_spi_device_t *pico_spi);
static uint winbond_init_spi(pico_spi_device_t *pico_spi);
static void winbond_init_dma(pico_spi_device_t *pico_spi, uint32_t initial_transfer_count);

static void winbond_init_gpio(pico_spi_device_t *pico_spi) {

    gpio_set_function(pico_spi->PIN_CS,   GPIO_FUNC_SIO);
    gpio_set_function(pico_spi->PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(pico_spi->PIN_MOSI, GPIO_FUNC_SPI);
    gpio_set_function(pico_spi->PIN_MISO, GPIO_FUNC_SPI);

    gpio_set_slew_rate(pico_spi->PIN_CS, GPIO_SLEW_RATE_FAST);
    gpio_set_slew_rate(pico_spi->PIN_SCK, GPIO_SLEW_RATE_FAST);
    gpio_set_slew_rate(pico_spi->PIN_MOSI, GPIO_SLEW_RATE_FAST);
    
    gpio_set_drive_strength(pico_spi->PIN_CS, GPIO_DRIVE_STRENGTH_8MA);
    gpio_set_drive_strength(pico_spi->PIN_SCK, GPIO_DRIVE_STRENGTH_8MA);
    gpio_set_drive_strength(pico_spi->PIN_MOSI, GPIO_DRIVE_STRENGTH_8MA);

    gpio_init(pico_spi->PIN_CS);
    gpio_set_dir(pico_spi->PIN_CS, GPIO_OUT);
    gpio_put(pico_spi->PIN_CS, W25Q_CHIP_SELECT_FALSE); // Set Winbond Chip Select = false
  
}

static void winbond_init_dma_tx(pico_spi_device_t *pico_spi) {

    dma_channel_config_t dma_channel_config_tx;
    dma_channel_config_tx = dma_channel_get_default_config(pico_spi->dma_channel_tx);
    uint dma_channel_dreq_tx = spi_get_dreq(pico_spi->device, true);
    volatile void *dma_write_address = &spi_get_hw(pico_spi->device)->dr;

    channel_config_set_transfer_data_size(&dma_channel_config_tx, DMA_SIZE_8);
    channel_config_set_read_increment(&dma_channel_config_tx, false); 
    channel_config_set_write_increment(&dma_channel_config_tx, false); 
    channel_config_set_dreq(&dma_channel_config_tx, dma_channel_dreq_tx); 

    dma_channel_set_irq0_enabled(pico_spi->dma_channel_tx, false);
    dma_channel_set_irq1_enabled(pico_spi->dma_channel_tx, false);
    dma_channel_set_read_addr(pico_spi->dma_channel_tx, NULL, false);
    dma_channel_set_write_addr(pico_spi->dma_channel_tx, dma_write_address, false);
    dma_channel_set_transfer_count(pico_spi->dma_channel_tx, pico_spi->DMA_BYTES, false);
    dma_channel_set_config(pico_spi->dma_channel_tx, &dma_channel_config_tx, false);

    printf("Initialising DMA, Winbond TX channel number=%d, Initial DMA Bytes=%ld\n", 
        pico_spi->dma_channel_tx, pico_spi->DMA_BYTES);

}

static void winbond_init_dma_rx(pico_spi_device_t *pico_spi) {

    dma_channel_config_t dma_channel_config_rx;
    dma_channel_config_rx = dma_channel_get_default_config(pico_spi->dma_channel_rx);
    uint dma_channel_dreq_rx = spi_get_dreq(pico_spi->device, false);
    volatile void *dma_read_address = &spi_get_hw(pico_spi->device)->dr;

    channel_config_set_transfer_data_size(&dma_channel_config_rx, DMA_SIZE_8);
    channel_config_set_read_increment(&dma_channel_config_rx, false);
    channel_config_set_write_increment(&dma_channel_config_rx, true);
    channel_config_set_dreq(&dma_channel_config_rx, dma_channel_dreq_rx);
   
    dma_channel_set_irq0_enabled(pico_spi->dma_channel_rx, true);
    dma_channel_set_irq1_enabled(pico_spi->dma_channel_rx, false);
    dma_channel_set_read_addr(pico_spi->dma_channel_rx, dma_read_address, false);
    
    dma_channel_set_write_addr(pico_spi->dma_channel_rx, NULL, false);
    dma_channel_set_transfer_count(pico_spi->dma_channel_rx, pico_spi->DMA_BYTES, false);
    dma_channel_set_config(pico_spi->dma_channel_rx, &dma_channel_config_rx, false);

    printf("Initialising DMA, Winbond RX channel number=%d, Initial DMA Bytes=%ld\n", 
       pico_spi->dma_channel_rx,  pico_spi->DMA_BYTES);
}

static void winbond_init_dma(pico_spi_device_t *pico_spi, uint32_t initial_transfer_count) {

    int dma_channel_tx = dma_claim_unused_channel(false);
    int dma_channel_rx = dma_claim_unused_channel(false);

    if (dma_channel_tx >= 0 && dma_channel_tx >= 0) {

        uint32_t dma_channel_mask_tx_rx = 0;
        
        dma_channel_mask_tx_rx |= (1u << (uint) dma_channel_tx);
        dma_channel_mask_tx_rx |= (1u << (uint) dma_channel_rx);

        pico_spi->DMA_BYTES = dma_encode_transfer_count(initial_transfer_count);

        pico_spi->dma_channel_tx = (uint)dma_channel_tx;
        pico_spi->dma_channel_rx = (uint)dma_channel_rx;
        pico_spi->dma_channel_mask_tx_rx = dma_channel_mask_tx_rx;

        winbond_init_dma_tx(pico_spi);
        winbond_init_dma_rx(pico_spi);

    } else {

        printf("Error Initialising DMA, unable to claim free TX & RX channels\n");

    }

}

static uint winbond_init_spi(pico_spi_device_t *pico_spi) {
   
    // For correct operation of Winbond W25Q series flash memory
    // ========================================================= 
    //
    // Conditions below are for breadboard/breakout wiring style
    // using Adafruit W25Q64JV Flash Breakout with buffering and
    // level shifters supporting 3V or 5V power and SPI logic.
    //
    // 1. SPI Clock Polarity >>> must be SPI_CPOL_0 
    // 2. SPI Clock Phase    >>> must be SPI_CPHA_1 
    //
    // Data read/write errors may be observed if 1 & 2 are set incorrectly
    // Note that this may only become apparent at higher clock frequencies
    // i.e. flash may read correctly at low clock frequencies (say 100kHz)
    // when SPI clock polarity and/or phase are set incorrectly or left at
    // their respective default values.
    //
    // Further testing is recommended if using flash on custom PCB designs
    // or with other buffered and/or level shifted flash breakout boards.

    uint spi_baud_requested = pico_spi->SPI_BAUD;

    uint spi_baud_actual = spi_init(pico_spi->device, spi_baud_requested);

    pico_spi->SPI_BAUD = spi_baud_actual;

    spi_set_format(pico_spi->device, SPI_BITS_8, SPI_CPOL_0, SPI_CPHA_1, SPI_MSB_FIRST);

    printf("Initialising Pico SPI, Baud Requested/Actual = %d/%d\n", spi_baud_requested, spi_baud_actual);
  
    return spi_baud_actual;
}

void winbond_init_all(pico_spi_device_t *pico_spi) {

    const uint32_t initial_dma_bytes = 512;
    
    printf("Winbond Initialising All, Initial DMA Bytes=%ld\n", initial_dma_bytes);

    winbond_init_gpio(pico_spi);

    winbond_init_dma(pico_spi, initial_dma_bytes);

    winbond_init_spi(pico_spi);

}