#ifndef SELECTOR_H
#define SELECTOR_H

#include "i_pop_up.h"

#include "bn_vector.h"
#include "bn_sprite_ptr.h"
#include "bn_optional.h"
#include "bn_string_view.h"
#include "bn_regular_bg_ptr.h"
#include "bn_sprite_tiles_ptr.h"
#include "bn_regular_bg_map_ptr.h"
#include "bn_sprite_text_generator.h"

namespace openflash
{
    class selector : public i_pop_up
    {
      private:
        bn::sprite_text_generator _text_generator;
        bn::vector<bn::sprite_ptr, 32> _text_sprites;
        bn::optional<bn::regular_bg_ptr> _selector_bg;
        bn::optional<bn::regular_bg_map_ptr> _bg_map;
        selector *_instance;

      public:
        selector();
        virtual ~selector() = default;
        virtual void render();
        virtual void update();
        virtual void dismiss();
        void update_position(int index);
    };
} // namespace openflash

#endif // SELECTOR_H