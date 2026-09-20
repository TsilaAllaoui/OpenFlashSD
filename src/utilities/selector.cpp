#include "selector.h"
#include "selector_bg.h"

#include "bn_bg_tiles.h"
#include "bn_display.h"
#include "bn_regular_bg_item.h"
#include "bn_regular_bg_map_ptr.h"
#include "bn_regular_bg_tiles_items_tiles.h"

#include "common_variable_8x16_sprite_font.h"
#include "utilities/text_helpers.h"

constexpr int selector_width = 48;
constexpr int title_offset_y = 12;

constexpr bn::array<bn::point, 4> selector_positions = {bn::point(11, 88),
                                                        bn::point(67, 88),
                                                        bn::point(125, 88),
                                                        bn::point(180, 88)};

constexpr bn::array<bn::string_view, 4> titles = {" Flash Cart",
                                                  "Cart Dump",
                                                  "Save Manager",
                                                  "Settings"};

namespace openflash
{
    selector::selector()
        : _text_generator(common::variable_8x16_sprite_font),
          _text_sprites(),
          _selector_bg()
    {
        _text_generator.set_bg_priority(0);
        _text_generator.set_center_alignment();
    }

    void selector::render()
    {
        if (_selector_bg)
        {
            return;
        }

        bn::bg_tiles::set_allow_offset(false);

        _selector_bg.emplace(bn::regular_bg_item(bn::regular_bg_tiles_items::tiles,
                                                 bn::regular_bg_tiles_items::tiles_palette,
                                                 openflash::selector_bg_map_item)
                                 .create_bg(0, 0));

        bn::regular_bg_map_ptr selector_map = _selector_bg->map();

        selector_map.reload_cells_ref();

        _selector_bg->set_priority(1);

        bn::bg_tiles::set_allow_offset(true);

        update_position(0);
    }

    void selector::update()
    {
    }

    void selector::dismiss()
    {
        _text_sprites.clear();
        _selector_bg.reset();
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
                                       text_x,
                                       text_y,
                                       _text_sprites);
    }
}