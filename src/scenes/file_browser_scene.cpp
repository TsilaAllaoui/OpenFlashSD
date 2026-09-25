#include "bn_bg_tiles.h"
#include "bn_regular_bg_item.h"
#include "bn_regular_bg_map_ptr.h"
#include "bn_sprite_items_sdcard.h"
#include "bn_regular_bg_tiles_items_tiles.h"

#include "flash_context.h"
#include "file_browser_bg.h"
#include "file_browser_scene.h"
#include "scene_state_machine.h"
#include "utilities/text_helpers.h"
#include "common_variable_8x16_sprite_font.h"

namespace openflash
{
    file_brower_scene::file_brower_scene()
        : _file_browser(),
          _loading_popup(),
          _type(scene_type::FILE_BROWSER),
          _background(),
          _text_generator(bn::sprite_text_generator(common::variable_8x16_sprite_font)),
          _sdcard_sprite(bn::sprite_items::sdcard.create_sprite(
              screen_left + 12,
              screen_top + 12)),
          _previous_file_browser(),
          _loading_files(false)
    {
        _sdcard_sprite.set_visible(false);
    }

    void file_brower_scene::enter()
    {
        _sdcard_sprite.set_visible(true);

        text_helpers::draw_centered(_text_generator,
                                    "File Browser",
                                    screen_top + 12,
                                    _text_sprites);

        bn::bg_tiles::set_allow_offset(false);
        _background.emplace(bn::regular_bg_item(
                                bn::regular_bg_tiles_items::tiles,
                                bn::regular_bg_tiles_items::tiles_palette,
                                openflash::file_browser_bg_map_item)
                                .create_bg(0, 0));
        _background->set_top_left_position(0, 0);
        bn::regular_bg_map_ptr bg_map_ptr = _background->map();
        bg_map_ptr.reload_cells_ref();
        bn::bg_tiles::set_allow_offset(true);

        _file_browser.emplace(flash_context::instance().get_current_file_filter());
        _file_browser->request_files();

        set_content_priority(1);
        _loading_popup.emplace("Loading files...", false, false);
        _loading_popup->render();
        _loading_files = true;
    }

    void file_brower_scene::exit()
    {
        _sdcard_sprite.set_visible(false);
        _text_sprites.clear();
        _loading_popup.reset();
        _loading_files = false;

        if (_file_browser &&
            _file_browser->restore_browser_state() &&
            scene_state_machine::instance().get_last_request_scene() != scene_type::MAIN_MENU)
        {
            _previous_file_browser = _file_browser->get_snapshot();
        }
        else
        {
            _previous_file_browser.reset();
        }

        _file_browser.reset();
        _background.reset();
    }

    void file_brower_scene::update()
    {
        if (!_file_browser)
            return;

        if (_loading_files)
        {
            if (_file_browser->update_file_loading())
            {
                _loading_popup.reset();
                _loading_files = false;
                set_content_priority(1);

                bool restored = false;

                if (_previous_file_browser && _file_browser->restore_browser_state())
                    restored = _file_browser->restore_snapshot(*_previous_file_browser);

                if (!restored)
                    _file_browser->render_file_list();
            }

            return;
        }

        _file_browser->update();
    }

    void file_brower_scene::render()
    {
        if (_file_browser && !_loading_files)
            _file_browser->render_file_list();
    }

    void file_brower_scene::set_content_priority(int priority)
    {
        if (_background)
            _background->set_priority(priority);

        _sdcard_sprite.set_bg_priority(priority);

        for (auto &sprite : _text_sprites)
            sprite.set_bg_priority(priority);
    }

    scene_type file_brower_scene::get_scene_type()
    {
        return _type;
    }

    void file_brower_scene::delete_file_browser_snapshot()
    {
        _previous_file_browser.reset();
    }

    void file_brower_scene::set_title(const bn::string_view &title)
    {
        _title = title;
    }
}
