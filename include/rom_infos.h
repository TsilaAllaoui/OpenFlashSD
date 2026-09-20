#ifndef ROM_INFOS_H
#define ROM_INFOS_H

#include "bn_optional.h"
#include "bn_string.h"

#include "file_entry.h"

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

    struct rom_infos
    {
        bn::string<max_file_patch_character> file_path;
        bn::string<12> name;
        bn::string<4> game_code;
        bn::string<2> maker_code;
        uint8_t complement_checksum;
        bool header_valid;
        save_type savetype;
    };
}

#endif // ROM_INFOS_H
