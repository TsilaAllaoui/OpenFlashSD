#ifndef ROM_INFOS_H
#define ROM_INFOS_H

#include "bn_optional.h"
#include "bn_string.h"

#include "file_entry.h"

constexpr int max_file_patch_character = 100;

namespace openflash
{
    struct rom_infos
    {
        bn::string<max_file_patch_character> file_path;
        bn::string<12> name;
        bn::string<4> game_code;
        bn::string<2> maker_code;
        uint8_t complement_checksum;
        bool header_valid;
    };
}

#endif // ROM_INFOS_H
