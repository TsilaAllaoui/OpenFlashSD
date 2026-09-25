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
            bn::optional<cart_infos> _current_cart_infos;
            bool _loading;
            bool _response_ready;
#ifdef USEMOCK
            int _mock_frames;
#endif

            cart_api();

        public:
            ~cart_api() = default;
            static cart_api &instance();
            void request_cart_infos();
            void update();
            bool response_available() const;
            const cart_infos &get_cart_infos_response() const;
        };
    }
}

#endif // CART_API_H
