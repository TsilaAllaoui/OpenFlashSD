#ifndef POP_UP_H
#define POP_UP_H

#include "i_pop_up.h"

#include "bn_vector.h"
#include "bn_sprite_tiles_ptr.h"
#include "bn_sprite_ptr.h"
#include "bn_optional.h"
#include "bn_string_view.h"
#include "bn_regular_bg_ptr.h"
#include "bn_sprite_text_generator.h"

namespace openflash
{
    class pop_up : public i_pop_up
    {
    private:
        bn::string_view _title;
        bn::optional<bn::regular_bg_ptr> _pop_up_bg;
        bn::sprite_text_generator _text_generator_8x16;
        bn::sprite_text_generator _text_generator_8x8;
        bn::vector<bn::sprite_ptr, 32> _text_sprites;
        bn::sprite_ptr *_cursor_sprite_ptr;
        bn::optional<bn::fixed> _old_cursor_pos;
        bool _confirmation_response;
        bool _cancellable;
        bool _acceptable;
        bool _open;

    public:
        pop_up(const bn::string_view &title,
               bool cancellable = true,
               bool acceptable = false,
               bn::sprite_ptr *cursor_sprite_ptr = nullptr);
        virtual ~pop_up();
        virtual void render();
        virtual void update();
        virtual void dismiss();
        bool get_confirmation_response() const;
        bool is_open() const;
    };
}

#endif // POP_UP_H
