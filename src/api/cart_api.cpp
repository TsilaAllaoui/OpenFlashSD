#include "pop_up.h"
#include "cart_api.h"
#include "utilities/async.h"

namespace openflash
{
    namespace api
    {
        cart_api &cart_api::instance()
        {
            static cart_api api;
            return api;
        }

        bn::optional<cart_infos> cart_api::get_current_cart_infos()
        {
            auto popup = pop_up("Getting cart infos...", false);

            // simulating wait time
            async::delay(60);

            // get current cart infos from server side (esp32)
            _current_cart_infos.emplace();
            _current_cart_infos->name = "M36L0T705";
            return _current_cart_infos;
        }
    }
}
