#include "bn_array.h"
#include "bn_string_view.h"
#include <cstdint>

#include "pop_up.h"
#include "rom_infos.h"
#include "rom_info_api.h"
#include "utilities/async.h"
#include "mock/mocks.h"

namespace openflash
{
    namespace api
    {
        rom_info_api &api::rom_info_api::instance()
        {
            static rom_info_api api;
            return api;
        }

        bn::optional<rom_infos> api::rom_info_api::get_current_rom_infos(const file_entry &file)
        {
            auto popup = pop_up("Getting rom infos...", false);

            // simulate wait time
            async::delay(60);

            // get current rom infos from api (esp32)
            auto header = mock::get_gba_header(file.path);
            auto rom_infos = mock::get_gba_file_info(header.data());
            rom_infos.file_path = file.path;

            _current_rom_infos.emplace(rom_infos);
            return _current_rom_infos;
        }
    }
}
