#pragma once
#include <stdbool.h>
#include "sdmmc_cmd.h"

/**
 * @brief Initialize SD card over SPI and mount FAT filesystem
 * @return true if successful, false otherwise
 */
bool sdcard_init(void);

/**
 * @brief List all files in the root directory
 */
void sdcard_list_files(void);

/**
 * @brief Unmount SD card and free SPI bus
 */
void sdcard_deinit(void);

/**
 * @brief Get pointer to sdmmc_card_t structure
 */
sdmmc_card_t *sdcard_get_card(void);
