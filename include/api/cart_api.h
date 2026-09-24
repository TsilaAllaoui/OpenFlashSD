#ifndef CART_API_H
#define CART_API_H

#include "bn_optional.h"
#include "cart_infos.h"

namespace openflash
{
    namespace api
    {
        class cart_api
        {
          private:
            cart_api();

            bn::optional<cart_infos> _current_cart_infos;
            bool _loading;
            bool _response_ready;
#ifdef USEMOCK
            int _mock_frames;
#endif

          public:
            ~cart_api() = default;
            static cart_api &instance();
            void request_cart_infos();
            void update();
            bool response_available() const;
            const cart_infos &get_cart_infos_response() const;
        };
    } // namespace api
} // namespace openflash

#endif // CART_API_H
