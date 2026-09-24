#ifndef ROM_INFO_API_H
#define ROM_INFO_API_H

#include "bn_optional.h"
#include "rom_infos.h"

namespace openflash
{
    namespace api
    {
        class rom_info_api
        {
          private:
            rom_info_api() = default;
            bn::optional<rom_infos> _current_rom_infos;

          public:
            ~rom_info_api() = default;
            static rom_info_api &instance();
            bn::optional<rom_infos> get_current_rom_infos(const file_entry &file);
        };
    } // namespace api
} // namespace openflash

#endif // ROM_INFO_API_H
