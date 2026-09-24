#include "bn_array.h"
#include "bn_string_view.h"
#include <cstdint>

#ifdef USEMOCK
#include "mock/mocks.h"
#endif

#include "rom_infos.h"
#include "rom_info_api.h"

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
            _current_rom_infos = infos;
            return _current_rom_infos;
#else
            // TODO: request ROM info from the ESP32.
            _current_rom_infos.reset();
            return bn::nullopt;
#endif
        }
    } // namespace api
} // namespace openflash
