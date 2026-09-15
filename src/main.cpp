#include "bn_log.h"
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

#include "bn_regular_bg_tiles_items_tiles.h"

#include "tile_maps.h"
#include "file_entry.h"
#include "file_browser.h"
#include "common_variable_8x16_sprite_font.h"

int main()
{
    bn::core::init();

    // Background
    bn::bg_tiles::set_allow_offset(false);

    bn::regular_bg_item bg_item(
        bn::regular_bg_tiles_items::tiles,
        bn::regular_bg_tiles_items::tiles_palette,
        openflash::tile_maps_map_item);

    bn::regular_bg_ptr bg =
        bg_item.create_bg(0, 0);

    bg.set_top_left_position(0, 0);

    bn::regular_bg_map_ptr bg_map_ptr = bg.map();
    bg_map_ptr.reload_cells_ref();

    bn::bg_tiles::set_allow_offset(true);

    // Getting files from the source
    openflash::file_browser file_browser;
    file_browser.load_files();
    
    while (true)
    {
        file_browser.render();
        bn::core::update();
    }
}