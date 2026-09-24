#ifndef COLOR_HELPERS_H
#define COLOR_HELPERS_H

#include "bn_color.h"
#include "bn_regular_bg_ptr.h"

namespace openflash
{
    namespace color_helpers
    {
        void replace_bg_color(bn::regular_bg_ptr &bg, bn::color old_color, bn::color new_color);
    }
} // namespace openflash

#endif // COLOR_HELPERS_H