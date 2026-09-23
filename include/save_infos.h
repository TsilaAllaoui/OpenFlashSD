#ifndef SAVE_INFO_H
#define SAVE_INFO_H

#include "bn_string.h"
#include "file_entry.h"
#include "process_infos.h"

constexpr int max_file_patch_character = 100;

namespace openflash
{
    enum class save_type
    {
        NONE = 0,
        EEPROM_512B, // 4 Kbit
        EEPROM_8K,   // 64 Kbit
        SRAM_32K,    // 256 Kbit
        FLASH_64K,   // 512 Kbit
        FLASH_128K,  // 1 Mbit
        FRAM_32K,
        FRAM_64K,
        FRAM_128K,
        UNKNOWN
    };

    struct save_infos
    {
        file_entry file;
        save_type type;
        process_status status;
    };
}

#endif // SAVE_INFO_H