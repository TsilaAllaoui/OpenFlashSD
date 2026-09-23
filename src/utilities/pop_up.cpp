#include "pop_up.h"
#include "pop_up_bg.h"

#include "bn_keypad.h"
#include "bn_display.h"
#include "bn_bg_tiles.h"
#include "bn_sprite_ptr.h"
#include "bn_regular_bg_item.h"
#include "bn_regular_bg_map_ptr.h"
#include "utilities/text_helpers.h"
#include "bn_regular_bg_tiles_items_tiles.h"
#include "common_variable_8x16_sprite_font.h"
#include "common_variable_8x8_sprite_font.h"

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
          _text_generator_8x16(bn::sprite_text_generator(common::variable_8x16_sprite_font)),
          _text_generator_8x8(bn::sprite_text_generator(common::variable_8x8_sprite_font)),
          _text_sprites(),
          _cursor_sprite_ptr(cursor_sprite_ptr),
          _old_cursor_pos(),
          _confirmation_response(false),
          _cancellable(cancellable),
          _acceptable(acceptable),
          _open(true)
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

        text_helpers::draw_centered(_text_generator_8x16,
                                    _title,
                                    -title_y - 8,
                                    _text_sprites);

        bn::string<64> body_text;

        if (_acceptable)
        {
            body_text += "A: Accept";
            if (_cancellable)
                body_text += "   B: back";
        }
        else if (_cancellable)
        {
            body_text += "B: back";
        }
        else
        {
            body_text += "Please wait...";
        }

        text_helpers::draw_centered_at(_text_generator_8x8,
                                       body_text,
                                       title_x,
                                       title_y,
                                       _text_sprites);

        for (auto &sprite : _text_sprites)
            sprite.set_bg_priority(0);
    }

    pop_up::~pop_up()
    {
        dismiss();
    }

    void pop_up::render()
    {
        if (_cursor_sprite_ptr && !_old_cursor_pos.has_value())
        {
            _old_cursor_pos.emplace(_cursor_sprite_ptr->y());
            _cursor_sprite_ptr->set_y(bn::display::width());
        }
    }

    void pop_up::update()
    {
        if (!_open)
            return;

        if (_cancellable && bn::keypad::b_pressed())
        {
            _confirmation_response = false;
            dismiss();
            return;
        }

        if (_acceptable && bn::keypad::a_pressed())
        {
            _confirmation_response = true;
            dismiss();
        }
    }

    void pop_up::dismiss()
    {
        if (!_open)
            return;

        _open = false;
        _pop_up_bg.reset();
        _text_sprites.clear();

        if (_cursor_sprite_ptr && _old_cursor_pos.has_value())
        {
            _cursor_sprite_ptr->set_y(_old_cursor_pos.value());
            _old_cursor_pos.reset();
        }
    }

    bool pop_up::get_confirmation_response() const
    {
        return _confirmation_response;
    }

    bool pop_up::is_open() const
    {
        return _open;
    }
}
