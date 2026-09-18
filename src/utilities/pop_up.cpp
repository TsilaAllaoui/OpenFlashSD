#include "pop_up.h"
#include "pop_up_bg.h"

#include "bn_core.h"
#include "bn_keypad.h"
#include "bn_display.h"
#include "bn_bg_tiles.h"
#include "bn_sprite_ptr.h"
#include "bn_regular_bg_item.h"
#include "bn_regular_bg_map_ptr.h"
#include "bn_regular_bg_tiles_items_tiles.h"
#include "common_variable_8x16_sprite_font.h"

constexpr int title_x = -4;
constexpr int title_y = 16;

namespace openflash
{
    pop_up::pop_up(const bn::string_view& title, bn::sprite_ptr* cursor_sprite_ptr)
        : _title(title),
          _pop_up_bg(),
          _text_generator(bn::sprite_text_generator(common::variable_8x16_sprite_font)),
          _text_sprites(),
          _cursor_sprite_ptr(cursor_sprite_ptr)
    {
        bn::bg_tiles::set_allow_offset(false);
            _pop_up_bg.emplace(bn::regular_bg_item(
                                   bn::regular_bg_tiles_items::tiles,
                                   bn::regular_bg_tiles_items::tiles_palette,
                                   openflash::pop_up_bg_map_item)
                                   .create_bg(0, 0));
            bn::regular_bg_map_ptr popup_map_ptr = _pop_up_bg.value().map();
            popup_map_ptr.reload_cells_ref();
            _pop_up_bg->set_priority(0);
            bn::bg_tiles::set_allow_offset(true);

            _text_generator.set_center_alignment();
            _text_generator.generate(title_x, -title_y, _title, _text_sprites);
            _text_generator.generate(title_x, title_y, "Press B to go back", _text_sprites);
            
            for (auto& sprite : _text_sprites)
            {
                sprite.set_bg_priority(0);
            }
    }

    void pop_up::render()
    {
        if (_cursor_sprite_ptr)
        {
            _old_cursor_pos.emplace(_cursor_sprite_ptr->y());
            _cursor_sprite_ptr->set_y(bn::display::width());
        }
    }

    void pop_up::update()
    {
        while (true)
        {
            bn::core::update();
            if(bn::keypad::b_pressed())
            {
                dismiss();
                break;
            }
        }
    }

    void pop_up::dismiss()
    {
        _pop_up_bg.reset();

        if (_cursor_sprite_ptr && _old_cursor_pos.has_value())
            _cursor_sprite_ptr->set_y(_old_cursor_pos.value());
    }
}
