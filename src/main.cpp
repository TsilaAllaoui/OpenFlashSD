#include "bn_core.h"
#include "bn_memory.h"
#include "bn_string.h"
#include "bn_keypad.h"
#include "bn_display.h"
#include "bn_bg_tiles.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_item.h"
#include "bn_regular_bg_ptr.h"
#include "bn_regular_bg_item.h"
#include "bn_regular_bg_map_ptr.h"
#include "bn_sprite_palette_ptr.h"
#include "bn_regular_bg_map_cell.h"
#include "bn_regular_bg_map_item.h"
#include "bn_sprite_text_generator.h"
#include "bn_regular_bg_map_cell_info.h"

#include "bn_sprite_items_sdcard.h"
#include "bn_sprite_items_cursor.h"
#include "bn_regular_bg_tiles_items_tiles.h"
#include "common_variable_8x16_sprite_font.h"

namespace
{
    struct bg_map
    {
        static constexpr int columns = 32;
        static constexpr int rows = 32;
        static constexpr int tile_count = 3;

        alignas(int) bn::regular_bg_map_cell cells[columns * rows];
        bn::regular_bg_map_item map_item;

        bg_map() :
            map_item(cells[0], bn::size(columns, rows))
        {
            bn::memory::clear(cells);
        }

        void set_tile(int x, int y, int tile_index, bool vertical_flip = false, bool horizontal_flip = false)
        {
            bn::regular_bg_map_cell& cell = cells[map_item.cell_index(x, y)];
            bn::regular_bg_map_cell_info info(cell);

            info.set_tile_index(tile_index);
            info.set_palette_id(0);
            info.set_vertical_flip(vertical_flip);
            info.set_horizontal_flip(horizontal_flip);
            cell = info.cell();
        }
    };
}

int main()
{
    // Init butano
    bn::core::init();

    // sprites
    bn::sprite_ptr sdcard_sprite_ptr = bn::sprite_items::sdcard.create_sprite(-(bn::display::width() / 2) + 12, -(bn::display::height() / 2) + 12);
    bn::sprite_palette_ptr shared_palette = sdcard_sprite_ptr.palette();
    bn::sprite_ptr cursor_sprite_ptr = bn::sprite_items::cursor.create_sprite(-(bn::display::width() / 2) + 14, -(bn::display::height() / 2) + 35);
    cursor_sprite_ptr.set_palette(shared_palette);

    // texts
    bn::sprite_text_generator text_generator(common::variable_8x16_sprite_font);
    text_generator.set_left_alignment();

    // backgrounds
    bn::bg_tiles::set_allow_offset(false);
    constexpr int y_limit = 19;
    constexpr int x_limit = 29;
    bg_map map;

    for(int y = 0; y < bg_map::rows; ++y)
    {
        for(int x = 0; x < bg_map::columns; ++x)
        {
            if (x == 0 && y == 0) 
                map.set_tile(x, y, 0);
            else if (x == 0 && y == y_limit) 
                map.set_tile(x, y, 0, true, false);
            else if (x == x_limit && y == 0)
                map.set_tile(x, y, 0, false, true);
            else if (x == x_limit && y == y_limit)
                map.set_tile(x, y, 0, true, true);
            else if (x == 0 || x == x_limit)
                map.set_tile(x, y, 2, false, x == x_limit);
            else if (y == 0 || y == y_limit)
                map.set_tile(x, y, 1, y == y_limit, false);
            else if (y >= 1 && y <= 2)
                map.set_tile(x, y, 4);
            else map.set_tile(x, y, 3);
        }
    }

    bn::regular_bg_item bg_item(
        bn::regular_bg_tiles_items::tiles,
        bn::regular_bg_tiles_items::tiles_palette,
        map.map_item
    );

    bn::regular_bg_ptr bg = bg_item.create_bg(0, 0);

    bg.set_top_left_position(0, 0);

    bn::regular_bg_map_ptr bg_map_ptr = bg.map();
    bg_map_ptr.reload_cells_ref();

    bn::bg_tiles::set_allow_offset(true);

    // files (placeholder, this should be get from esp32 side to link port via GPIO)
    bn::vector<bn::string_view, 10> files;
    files.push_back("CONFIG");
    files.push_back("GB");
    files.push_back("GBA");
    files.push_back("Rom1.gba");
    files.push_back("Rom2.gba");
    files.push_back("Rom3.gba");
    files.push_back("Rom4.gba");
    files.push_back("config.cfg");
    files.push_back("test");

    // current selectd file
    int current_selected_file = 0;

    int text_spacing_y = 10;

    while(true)
    {
        // display banner
        bn::vector<bn::sprite_ptr, 32> text_sprites;
        text_generator.generate(-(bn::display::width() / 2) + 80, -(bn::display::height() / 2) + 12, "OpenFlashSD", text_sprites);

        // displaying files
        int offset = 0;
        for (const auto& file : files) {
            text_generator.generate(-(bn::display::width() / 2) + 20, -(bn::display::height() / 2) + 35 + offset++ * text_spacing_y, file, text_sprites);
        }

        // key presses and navigation
        if (bn::keypad::down_pressed()) {
            if (current_selected_file < files.size() - 1) {
                current_selected_file++;
                cursor_sprite_ptr.set_y(cursor_sprite_ptr.y() + text_spacing_y);
            }
        }

        if (bn::keypad::up_pressed()) {
            if (current_selected_file > 0) {
                current_selected_file--;
                cursor_sprite_ptr.set_y(cursor_sprite_ptr.y() - text_spacing_y);
            }
        }

        bn::core::update();
    }
}