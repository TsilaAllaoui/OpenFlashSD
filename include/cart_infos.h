#ifndef CART_INFOS_H
#define CART_INFOS_H

#include "bn_string.h"

constexpr int max_cart_character_name = 100;

namespace openflash
{
    struct cart_infos
    {
        bn::string<max_cart_character_name> name;
    };
}

#endif // CART_INFOS_H