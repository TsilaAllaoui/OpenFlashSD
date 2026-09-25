#include "cart_api.h"
#include "bn_assert.h"

#ifdef USEMOCK
#include "bn_random.h"
#include "mock/mocks.h"
#endif

namespace openflash
{
    namespace api
    {
#ifdef USEMOCK
        namespace
        {
            bn::random cart_random;
        }
#endif

        cart_api::cart_api()
            : _current_cart_infos(),
              _loading(false),
              _response_ready(false)
#ifdef USEMOCK
              , _mock_frames(0)
#endif
        {
        }

        cart_api &cart_api::instance()
        {
            static cart_api api;
            return api;
        }

        void cart_api::request_cart_infos()
        {
            _current_cart_infos.reset();
            _loading = true;
            _response_ready = false;
#ifdef USEMOCK
            _mock_frames = 15;
#else
            // Request cart information from ESP32 here.
#endif
        }

        void cart_api::update()
        {
            if (!_loading)
                return;

#ifdef USEMOCK
            if (_mock_frames > 0)
            {
                _mock_frames--;
                return;
            }

            _current_cart_infos.emplace();
            int random_rom_infos_index = cart_random.get_int(mock::mock_roms_infos.size());
            const auto &file = mock::mock_roms_infos[random_rom_infos_index];
            auto header = mock::get_gba_header(file.path);
            _current_cart_infos->cart_rom_infos = mock::get_gba_file_info(header.data());
            _current_cart_infos->name = "M36L0T735";
            _loading = false;
            _response_ready = true;
#else
            // Poll ESP32 response here. Keep _loading true until a complete response is available.
#endif
        }

        bool cart_api::response_available() const
        {
            return _response_ready && _current_cart_infos.has_value();
        }

        const cart_infos &cart_api::get_cart_infos_response() const
        {
            if (!_current_cart_infos.has_value())
                BN_ERROR("Cart infos is nullopt");

            return *_current_cart_infos;
        }
    }
}
