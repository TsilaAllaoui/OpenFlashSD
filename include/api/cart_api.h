#ifndef CART_API_H
#define CART_API_H

#include "cart_infos.h"

#include "bn_optional.h"

namespace openflash
{
    namespace api
    {
        class cart_api
        {
        private:
            cart_api() = default;

            bn::optional<cart_infos> _current_cart_infos;

        public:
            ~cart_api() = default;
            static cart_api &instance();
            bn::optional<cart_infos> get_current_cart_infos();
        };
    }
}

#endif // CART_API_H