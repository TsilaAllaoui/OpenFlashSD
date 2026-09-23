#include "bn_vector.h"
#include "bn_random.h"

#include "cart_api.h"
#include "mock/mocks.h"
#include "file_entry.h"

namespace openflash
{
    namespace api
    {

        namespace
        {
            bn::random cart_random;
        }

        cart_api &cart_api::instance()
        {
            static cart_api api;
            return api;
        }

        bn::optional<cart_infos> cart_api::get_current_cart_infos()
        {
            // get current cart infos from server side (esp32)
            _current_cart_infos.emplace();
            _current_cart_infos->name = "M36L0T705";

            auto random_rom_infos_index = cart_random.get_int(mock::mock_roms_infos.size());
            auto file = mock::mock_roms_infos[random_rom_infos_index];
            auto header = mock::get_gba_header(file.path);
            _current_cart_infos->cart_rom_infos = mock::get_gba_file_info(header.data());
            return _current_cart_infos;
        }
    }
}
