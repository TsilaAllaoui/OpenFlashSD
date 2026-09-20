#include "pop_up.h"
#include "pop_up_bg.h"

#include "bn_core.h"
#include "bn_keypad.h"
#include "bn_display.h"
#include "bn_bg_tiles.h"
#include "bn_sprite_ptr.h"
#include "bn_regular_bg_item.h"
#include "bn_regular_bg_map_ptr.h"
#include "utilities/text_helpers.h"
#include "bn_regular_bg_tiles_items_tiles.h"
#include "common_variable_8x16_sprite_font.h"

constexpr int title_x = -4;
constexpr int title_y = 16;

namespace openflash
{
    pop_up::pop_up(const bn::string_view &title,
                   bool cancellable,
                   bool acceptable,
                   bn::sprite_ptr *cursor_sprite_ptr)
        : _title(title),
          _pop_up_bg(),
          _text_generator(bn::sprite_text_generator(common::variable_8x16_sprite_font)),
          _text_sprites(),
          _cursor_sprite_ptr(cursor_sprite_ptr),
          _confirmation_response(false)
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

        text_helpers::draw_centered_at(_text_generator,
                                       _title,
                                       title_x,
                                       -title_y + (!cancellable ? 15 : 0),
                                       _text_sprites);

        if (cancellable)
        {

            text_helpers::draw_centered_at(_text_generator,
                                           "Press B to go back",
                                           title_x,
                                           title_y,
                                           _text_sprites);
        }

        if (acceptable)
        {

            text_helpers::draw_centered_at(_text_generator,
                                           "Press A to continue",
                                           title_x,
                                           title_y - 17,
                                           _text_sprites);
        }

        for (auto &sprite : _text_sprites)
        {
            sprite.set_bg_priority(0);
        }
    }

    pop_up::~pop_up()
    {
        dismiss();
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
            if (bn::keypad::b_pressed())
            {
                _confirmation_response = false;
                dismiss();
                break;
            }
            if (bn::keypad::a_pressed())
            {
                _confirmation_response = true;
                dismiss();
                break;
            }
        }
    }

    void pop_up::dismiss()
    {
        _pop_up_bg.reset();
        _text_sprites.clear();

        if (_cursor_sprite_ptr && _old_cursor_pos.has_value())
            _cursor_sprite_ptr->set_y(_old_cursor_pos.value());
    }

    bool pop_up::get_confirmation_response()
    {
        return _confirmation_response;
    }
}
