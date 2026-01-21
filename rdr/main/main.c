#include <stdio.h>
#include "esp_log.h"
// #include "sdcard.h"
#include "it8951.h"

static const char *TAG = "MAIN";

void app_main(void)
{
    // if (!sdcard_init()) {
    //     ESP_LOGE(TAG, "SD card init failed");
    //     return;
    // }

    // sdcard_list_files();

    // // Example: open a BMP file in binary mode
    // FILE *f = fopen("/sdcard/080.bmp", "rb");
    // if (!f) {
    //     ESP_LOGE(TAG, "Failed to open BMP file");
    // } else {
    //     ESP_LOGI(TAG, "BMP file opened successfully");
    //     fclose(f);
    // }

    if (!it8951_init()) {
        ESP_LOGE(TAG, "Failed to it8951");
    }
    // it8951_clear_screen();

    // sdcard_deinit();
}

// #include <stdio.h>
// #include <dirent.h>
// #include "esp_log.h"
// #include "esp_err.h"
// #include "driver/spi_common.h"
// #include "driver/sdspi_host.h"
// #include "sdmmc_cmd.h"
// #include "esp_vfs_fat.h"

// static const char *TAG = "SD";

// void app_main(void)
// {
//     esp_err_t ret;

//     // 1. Configure SPI host
//     sdmmc_host_t host = SDSPI_HOST_DEFAULT();

//     spi_bus_config_t bus_cfg = {
//         .mosi_io_num = 47,
//         .miso_io_num = 13,
//         .sclk_io_num = 21,
//         .quadwp_io_num = -1,
//         .quadhd_io_num = -1,
//         .max_transfer_sz = 4000,
//     };

//     ret = spi_bus_initialize(host.slot, &bus_cfg, SPI_DMA_CH_AUTO);
//     if (ret != ESP_OK) {
//         ESP_LOGE(TAG, "SPI init failed");
//         return;
//     }

//     // 2. Configure SD device
//     sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
//     slot_config.gpio_cs = 48;
//     slot_config.host_id = host.slot;

//     // 3. Mount filesystem
    // esp_vfs_fat_sdmmc_mount_config_t mount_config = {
    //     .format_if_mount_failed = false,
    //     .max_files = 5,
    //     .allocation_unit_size = 16 * 1024
    // };

//     sdmmc_card_t *card;
//     ret = esp_vfs_fat_sdspi_mount(
//         "/sdcard",
//         &host,
//         &slot_config,
//         &mount_config,
//         &card
//     );

//     if (ret != ESP_OK) {
//         ESP_LOGE(TAG, "Failed to mount SD card");
//         return;
//     }

//     ESP_LOGI(TAG, "SD card mounted");

//     // 4. Read a file
//     FILE *f = fopen("/sdcard/080.bmp", "rb");
//     if (f == NULL) {
//         ESP_LOGE(TAG, "Failed to open file");
//         return;
//     }

//     // List files in root directory
//     // DIR *dir = opendir("/sdcard");
//     // if (!dir) {
//     //     ESP_LOGE(TAG, "Failed to open directory");
//     //     return;
//     // }

//     // struct dirent *entry;
//     // while ((entry = readdir(dir)) != NULL) {
//     //     ESP_LOGI(TAG, "Found file: %s", entry->d_name);
//     // }

//     // closedir(dir);

//     uint8_t header[54];  // BMP header size

//     size_t read = fread(header, 1, sizeof(header), f);
//     if (read != sizeof(header)) {
//         ESP_LOGE(TAG, "Failed to read BMP header");
//         fclose(f);
//         return;
//     }

//     ESP_LOGI(TAG, "BMP signature: %c%c", header[0], header[1]);

//     // Optional: unmount SD card
//     esp_vfs_fat_sdcard_unmount("/sdcard", card);
//     spi_bus_free(host.slot);

//     ESP_LOGI(TAG, "Done listing files");
// }
