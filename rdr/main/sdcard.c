#include "sdcard.h"
#include <stdio.h>
#include <dirent.h>
#include "esp_log.h"
#include "driver/sdspi_host.h"
#include "esp_vfs_fat.h"
#include "driver/spi_common.h"

static const char *TAG = "SDCARD";
static sdmmc_card_t *card = NULL;
static sdmmc_host_t host = SDSPI_HOST_DEFAULT();

static spi_bus_config_t bus_cfg = {
    .mosi_io_num = 47,
    .miso_io_num = 13,
    .sclk_io_num = 21,
    .quadwp_io_num = -1,
    .quadhd_io_num = -1,
    .max_transfer_sz = 4000,
};

static sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();

bool sdcard_init(void)
{
    esp_err_t ret;

    // Initialize SPI bus
    ret = spi_bus_initialize(host.slot, &bus_cfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "SPI bus init failed");
        return false;
    }

    slot_config.gpio_cs = 48;
    slot_config.host_id = host.slot;

    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };

    ret = esp_vfs_fat_sdspi_mount("/sdcard", &host, &slot_config, &mount_config, &card);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to mount SD card");
        spi_bus_free(host.slot);
        return false;
    }

    ESP_LOGI(TAG, "SD card mounted successfully");
    return true;
}

void sdcard_list_files(void)
{
    if (!card) {
        ESP_LOGE(TAG, "SD card not mounted");
        return;
    }

    DIR *dir = opendir("/sdcard");
    if (!dir) {
        ESP_LOGE(TAG, "Failed to open /sdcard directory");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        ESP_LOGI(TAG, "Found file: %s", entry->d_name);
    }

    closedir(dir);
}

void sdcard_deinit(void)
{
    if (!card) return;

    esp_vfs_fat_sdcard_unmount("/sdcard", card);
    spi_bus_free(host.slot);
    card = NULL;
    ESP_LOGI(TAG, "SD card unmounted");
}

sdmmc_card_t *sdcard_get_card(void)
{
    return card;
}
