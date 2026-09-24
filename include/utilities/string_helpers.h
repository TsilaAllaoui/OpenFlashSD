#ifndef STRING_HELPERS_H
#define STRING_HELPERS_H

#include "bn_string_view.h"

#include "rom_infos.h"

namespace openflash
{
    namespace string_helpers
    {
        constexpr bn::string_view to_string(save_type savetype)
        {
            switch (savetype)
            {
                case save_type::NONE:
                    return "NONE";

                case save_type::EEPROM_512B:
                    return "EEPROM 512B";

                case save_type::EEPROM_8K:
                    return "EEPROM 8K";

                case save_type::SRAM_32K:
                    return "SRAM 32K";

                case save_type::FLASH_64K:
                    return "FLASH 64K";

                case save_type::FLASH_128K:
                    return "FLASH 128K";

                case save_type::FRAM_32K:
                    return "FRAM 32K";

                case save_type::FRAM_64K:
                    return "FRAM 64K";

                case save_type::FRAM_128K:
                    return "FRAM 128K";

                case save_type::UNKNOWN:
                default:
                    return "UNKNOWN";
            }
        }
    } // namespace string_helpers
} // namespace openflash

#endif // STRING_HELPERS_H