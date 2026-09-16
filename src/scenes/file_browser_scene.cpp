
#include "bn_core.h"
#include "bn_bg_tiles.h"
#include "bn_regular_bg_item.h"
#include "bn_regular_bg_map_ptr.h"
#include "bn_regular_bg_tiles_items_tiles.h"

#include "tile_maps.h"
#include "scenes/file_browser_scene.h"
#include "common_variable_8x16_sprite_font.h"

namespace openflash
{
    file_brower_scene::file_brower_scene()
        : _type(scene_type::FILE_BROWSER), _background(bn::regular_bg_item(
                                                           bn::regular_bg_tiles_items::tiles,
                                                           bn::regular_bg_tiles_items::tiles_palette,
                                                           openflash::tile_maps_map_item)
                                                           .create_bg(0, 0)),
          _text_generator(bn::sprite_text_generator(common::variable_8x16_sprite_font)),
          _text_sprites(bn::vector<bn::sprite_ptr, 32>())
    {
        _file_browser.load_files();

        _text_generator.set_left_alignment();

        // header
        _text_generator.generate(
            screen_left + 80,
            screen_top + 12,
            "File Browser",
            _text_sprites);

        // Background
        bn::bg_tiles::set_allow_offset(false);
        _background.set_top_left_position(0, 0);
        bn::regular_bg_map_ptr bg_map_ptr = _background.map();
        bg_map_ptr.reload_cells_ref();
        bn::bg_tiles::set_allow_offset(true);
    }

    void file_brower_scene::render()
    {
        _file_browser.update();
        _file_browser.render_file_list();
    }

    scene_type file_brower_scene::get_scene_type()
    {
        return _type;
    }

    void file_brower_scene::render_background()
    {
        // Background
        bn::bg_tiles::set_allow_offset(false);

        bn::regular_bg_item bg_item(
            bn::regular_bg_tiles_items::tiles,
            bn::regular_bg_tiles_items::tiles_palette,
            openflash::tile_maps_map_item);

        _background =
            bg_item.create_bg(0, 0);

        _background.set_top_left_position(0, 0);

        bn::regular_bg_map_ptr bg_map_ptr = _background.map();
        bg_map_ptr.reload_cells_ref();

        bn::bg_tiles::set_allow_offset(true);
    }
}