#include "color_helpers.h"
#include "bn_bg_palette_ptr.h"

namespace openflash
{
    namespace color_helpers
    {
        void replace_bg_color(bn::regular_bg_ptr &bg, bn::color old_color, bn::color new_color)
        {
            bn::bg_palette_ptr palette = bg.palette();

            auto colors = palette.colors();

            for (int index = 0; index < colors.size(); ++index)
            {
                if (colors[index] == old_color)
                {
                    palette.set_color(index, new_color);
                }
            }
        }
    } // namespace color_helpers
} // namespace openflash