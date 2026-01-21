#include "it8951.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"

static const char *TAG = "IT8951";
static spi_device_handle_t it8951_20_spi;
static spi_device_handle_t it8951_02_spi;

DeviceInfo devInfo;

static void wait_for_ready(void){
    while (gpio_get_level(IT8951_HRDY) == 0) {
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

// static void spi_write_buffer()

static void spi_write16(uint16_t preamble, uint16_t data)
{
    uint8_t tx[4] = {
        preamble >> 8,
        preamble & 0xFF,
        data >> 8,
        data & 0xFF
    };

    spi_transaction_t t = {
        .length = 32,
        .tx_buffer = tx
    };

    wait_for_ready();  

    gpio_set_level(IT8951_CS, 0);

    esp_err_t ret = spi_device_transmit(it8951_02_spi, &t);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to write word data");
    }

    gpio_set_level(IT8951_CS, 1);
}



static uint16_t spi_read16(void){
    uint8_t tx[4] = {
        0x10,
        0x00,
        0x00,
        0x00
    };

    uint8_t rx[4];

    spi_transaction_t t = {
        .length = 32,
        .tx_buffer = tx,
        .rx_buffer = rx
    };

    wait_for_ready();  

    gpio_set_level(IT8951_CS, 0);

    esp_err_t ret = spi_device_transmit(it8951_02_spi, &t);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read word data");
    }

    gpio_set_level(IT8951_CS, 1);

    return rx[2] << 8 | rx[3];
}

static void spi_n_read16(uint16_t* rx_buffer, uint32_t word_count){
    uint8_t tx[4] = {
        0x10,
        0x00,
        0x00,
        0x00
    };

    uint8_t rx[2];

    spi_transaction_t t = {
        .length = 32,
        .rxlength = 16,
        .tx_buffer = tx,
        .rx_buffer = rx
    };

    wait_for_ready();  

    gpio_set_level(IT8951_CS, 0);

    esp_err_t ret = spi_device_transmit(it8951_02_spi, &t);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read word data");
    }

    uint8_t word_bytes[2] = {0};
    t.length = 16;        // 16 bits = 2 bytes
    t.tx_buffer = NULL;
    t.rx_buffer = word_bytes;
    for(uint32_t i=0; i<word_count; i++)
    {
        wait_for_ready();
        spi_device_transmit(it8951_02_spi, &t);

        // ESP_LOGI(TAG, "%d, %d = %d, %d",2*i, (2*i + 1), word_bytes[0], word_bytes[1]);
        rx_buffer[i] = ((uint16_t)word_bytes[0] << 8) | word_bytes[1];
    }

    gpio_set_level(IT8951_CS, 1);
}

static void write_cmd(uint16_t cmd_code)
{
    spi_write16(0x6000, cmd_code);
}

// static void display_area(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t mode)

// void clear_screen()
// {

// }

static void get_sys_info()
{
    write_cmd(IT8951_CMD_GET_DEV_INFO);

    uint16_t* rdWord = (uint16_t*)&devInfo;

    spi_n_read16(rdWord, sizeof(DeviceInfo)/2);

    ESP_LOGI(TAG, "Panel(W,H) = (%d,%d)",
    devInfo.PanelW, devInfo.PanelH );
    ESP_LOGI(TAG, "Image Buffer Address L = %X",
    devInfo.ImgBufAddrL);
    ESP_LOGI(TAG, "Image Buffer Address H = %X",
    devInfo.ImgBufAddrH);
    ESP_LOGI(TAG, "Image Buffer Address = %X",
    devInfo.ImgBufAddrL | (devInfo.ImgBufAddrH << 16));
    //Show Firmware and LUT Version
    ESP_LOGI(TAG, "FW Version = %s", (uint8_t*)devInfo.FWVersion);
    ESP_LOGI(TAG, "LUT Version = %s", (uint8_t*)devInfo.LUTVersion);
}

bool it8951_init(void){
    esp_err_t ret;

    spi_bus_config_t buscfg = {
        .mosi_io_num = IT8951_MOSI,
        .miso_io_num = IT8951_MISO,
        .sclk_io_num = IT8951_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        // .max_transfer_sz = 64 * 1024
    };

    ret = spi_bus_initialize(IT8951_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "SPI bus init failed");
        return false;
    }

    spi_device_interface_config_t fdevcfg = {
        .clock_speed_hz = 20 * 1000 * 1000, // 20 MHz
        .mode = 0,
        .spics_io_num = -1,
        .queue_size = 3,
        .flags = SPI_DEVICE_HALFDUPLEX
    };

    ret = spi_bus_add_device(IT8951_SPI_HOST, &fdevcfg, &it8951_20_spi);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Fast SPI device add failed");
        return false;
    }

    spi_device_interface_config_t sdevcfg = {
        .clock_speed_hz = 2 * 1000 * 1000, // 2 MHz
        .mode = 0,
        .spics_io_num = -1,
        .queue_size = 1,
    };

    ret = spi_bus_add_device(IT8951_SPI_HOST, &sdevcfg, &it8951_02_spi);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Slow SPI device add failed");
        return false;
    }

    gpio_config_t io_conf = {0};

    // CS pin as output
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << IT8951_CS);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);

    // RESET pin as output
    io_conf.pin_bit_mask = (1ULL << IT8951_RST);
    gpio_config(&io_conf);

    // HRDY pin as input
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << IT8951_HRDY);
    io_conf.pull_up_en = 0;
    io_conf.pull_down_en = 0;
    gpio_config(&io_conf);

    gpio_set_level(IT8951_RST, 0);
    vTaskDelay(pdMS_TO_TICKS(1000));
    gpio_set_level(IT8951_RST, 1);

    int freq_khz;
    spi_device_get_actual_freq(it8951_02_spi, &freq_khz);
    ESP_LOGI(TAG, "SPI device frequency %d KHz", freq_khz);

    spi_device_get_actual_freq(it8951_20_spi, &freq_khz);
    ESP_LOGI(TAG, "SPI device frequency %d KHz", freq_khz);

    size_t bus_max_transfer_sz;
    spi_bus_get_max_transaction_len(IT8951_SPI_HOST, &bus_max_transfer_sz);

    ESP_LOGI(TAG, "Allocating %d bytes for xfer buffers", bus_max_transfer_sz);

    get_sys_info();

    return true;
}