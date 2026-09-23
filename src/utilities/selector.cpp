#include "selector.h"
#include "selector_bg.h"

#include "bn_display.h"
#include "bn_bg_tiles.h"
#include "bn_bg_palette_ptr.h"
#include "bn_regular_bg_ptr.h"
#include "bn_regular_bg_item.h"
#include "bn_regular_bg_item.h"
#include "bn_regular_bg_map_ptr.h"
#include "bn_regular_bg_tiles_items_tiles.h"

#include "utilities/text_helpers.h"
#include "utilities/color_helpers.h"
#include "common_variable_8x16_sprite_font.h"

constexpr int selector_width = 48;
constexpr int title_offset_y = 12;

constexpr bn::array<bn::point, 3> selector_positions = {bn::point(19, 88),
                                                        bn::point(99, 88),
                                                        bn::point(172, 88)};

constexpr bn::array<bn::string_view, 3> titles = {" Flash Cart",
                                                  "Cart Dump",
                                                  "Save Manager"};

namespace openflash
{
    selector::selector()
        : _text_generator(common::variable_8x16_sprite_font),
          _text_sprites(),
          _selector_bg(),
          _bg_map()
    {
        bn::bg_tiles::set_allow_offset(false);

        auto bg_item = bn::regular_bg_item(bn::regular_bg_tiles_items::tiles,
                                           bn::regular_bg_tiles_items::tiles_palette,
                                           openflash::selector_bg_map_item);

        bn::regular_bg_ptr bg_ptr = bg_item.create_bg(0, 0);

        _selector_bg.emplace(bg_ptr);

        _bg_map.emplace(bg_ptr.map());

        _bg_map->reload_cells_ref();

        _selector_bg->set_priority(1);

        bn::bg_tiles::set_allow_offset(true);
    }

    void selector::render()
    {
        if (_selector_bg)
        {
            static bn::color first_color(0, 17, 17);
            static bn::color second_color(0, 25, 25);
            static int frames = 0;
            static bool change = false;
            frames++;
            if (frames >= 25)
            {
                frames = 0;
                if (!change)
                    color_helpers::replace_bg_color(*_selector_bg, first_color, second_color);
                else
                    color_helpers::replace_bg_color(*_selector_bg, second_color, first_color);
                change = !change;
            }
            return;
        }

        update_position(0);
    }

    void selector::update()
    {
    }

    void selector::dismiss()
    {
        _text_sprites.clear();
        _selector_bg.reset();
        _bg_map.reset();
    }

    void selector::update_position(int index)
    {
        if (!_selector_bg)
        {
            return;
        }

        if (index < 0 || index >= selector_positions.size())
        {
            return;
        }

        const bn::point &position = selector_positions[index];

        _selector_bg->set_top_left_position(position);

        _text_sprites.clear();

        const int text_x = position.x() - bn::display::width() / 2 + selector_width / 2;

        const int text_y = position.y() - bn::display::height() / 2 - title_offset_y;

        text_helpers::draw_centered_at(_text_generator,
                                       titles[index],
                                       text_x - (index == 2 ? 4 : 0),
                                       text_y,
                                       _text_sprites);
    }
}