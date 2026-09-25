#include "rom_info_api.h"

#ifdef USEMOCK
#include "mock/mocks.h"
#endif

namespace openflash
{
    namespace api
    {
        rom_info_api &rom_info_api::instance()
        {
            static rom_info_api api;
            return api;
        }

        bn::optional<rom_infos> rom_info_api::get_current_rom_infos(const file_entry &file)
        {
#ifdef USEMOCK
            auto header = mock::get_gba_header(file.path);
            auto infos = mock::get_gba_file_info(header.data());
            infos.file_path = file.path;
            _current_rom_infos.emplace(infos);
#else
            (void) file;
            _current_rom_infos.reset();
            // Request ROM information from ESP32 here.
#endif
            return _current_rom_infos;
        }
    }
}
