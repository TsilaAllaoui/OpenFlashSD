
#include "bn_core.h"
#include "bn_bg_tiles.h"
#include "bn_regular_bg_item.h"
#include "bn_regular_bg_map_ptr.h"
#include "bn_sprite_items_sdcard.h"
#include "bn_regular_bg_tiles_items_tiles.h"

#include "file_browser_bg.h"
#include "file_browser_scene.h"
#include "utilities/text_helpers.h"
#include "scenes/file_browser_scene.h"
#include "common_variable_8x16_sprite_font.h"

namespace openflash
{
    file_brower_scene::file_brower_scene()
        : _file_browser(),
          _type(scene_type::FILE_BROWSER),
          _background(),
          _text_generator(bn::sprite_text_generator(common::variable_8x16_sprite_font)),
          _sdcard_sprite(bn::sprite_items::sdcard.create_sprite(
              screen_left + 12,
              screen_top + 12))
    {
        _sdcard_sprite.set_visible(false);
    }

    void file_brower_scene::enter()
    {
        _sdcard_sprite.set_visible(true);

        // header
        text_helpers::draw_centered(_text_generator,
                                    "File Browser",
                                    screen_top + 12,
                                    _text_sprites);

        // Background
        bn::bg_tiles::set_allow_offset(false);
        _background.emplace(bn::regular_bg_item(
                                bn::regular_bg_tiles_items::tiles,
                                bn::regular_bg_tiles_items::tiles_palette,
                                openflash::file_browser_bg_map_item)
                                .create_bg(0, 0));
        _background.value().set_top_left_position(0, 0);
        bn::regular_bg_map_ptr bg_map_ptr = _background.value().map();
        bg_map_ptr.reload_cells_ref();
        bn::bg_tiles::set_allow_offset(true);

        _file_browser.emplace();
        _file_browser.value().load_files();

        if (_previous_file_browser && _file_browser->restore_browser_state())
        {
            _file_browser->restore_snapshot(
                *_previous_file_browser);
        }

    }

    void file_brower_scene::exit()
    {
        _sdcard_sprite.set_visible(false);
        _text_sprites.clear();

        if (_file_browser && _file_browser->restore_browser_state())
        {
            _previous_file_browser = _file_browser->get_snapshot();
        }

        _file_browser.reset();

        _background.reset();
    }

    void file_brower_scene::update()
    {
        if (_file_browser)
            _file_browser->update();
    }

    void file_brower_scene::render()
    {
        if (_file_browser)
            _file_browser->render_file_list();
    }

    scene_type file_brower_scene::get_scene_type()
    {
        return _type;
    }

    void file_brower_scene::delete_file_browser_snapshot()
    {
        _previous_file_browser.reset();
    }
}