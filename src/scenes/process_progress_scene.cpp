#include "bn_keypad.h"
#include "bn_core.h"
#include "bn_display.h"
#include "bn_bg_tiles.h"
#include "bn_regular_bg_item.h"
#include "bn_sprite_items_info.h"
#include "bn_sprite_items_save.h"
#include "bn_sprite_items_arrow.h"
#include "bn_regular_bg_map_ptr.h"
#include "bn_sprite_items_sdcard.h"
#include "bn_sprite_items_gbacart.h"

#include "file_entry.h"
#include "api/cart_api.h"
#include "flash_context.h"
#include "string_helpers.h"
#include "flash_screen_bg.h"
#include "utilities/pop_up.h"
#include "scene_state_machine.h"
#include "process_progress_scene_bg.h"
#include "utilities/text_helpers.h"
#include "bn_regular_bg_tiles_items_tiles.h"
#include "common_variable_8x16_sprite_font.h"
#include "process_progress_scene.h"

namespace openflash
{
    process_progress_scene::process_progress_scene()
        : _type(scene_type::PROCESS_PROGRESS),
          _background(),
          _pop_up_bg(),
          _text_generator(bn::sprite_text_generator(common::variable_8x16_sprite_font)),
          _sprites()
    {
        _sprites.emplace_back(bn::sprite_items::arrow.create_sprite(-5, -14));
        _sprites.emplace_back(bn::sprite_items::info.create_sprite(-bn::display::width() / 2 + 30, 60));

        for (auto &sprite : _sprites)
            sprite.set_visible(false);

        _current_process_infos.elapsed_time.minutes = 1;
        _current_process_infos.elapsed_time.seconds = 24;
        _current_process_infos.estimated_time.minutes = 5;
        _current_process_infos.estimated_time.seconds = 12;
        _current_process_infos.progress = 7;
        _current_process_infos.speed = 183;
        _current_process_infos.status = process_status::WRITING;
    }

    void process_progress_scene::update_screen_infos()
    {
        _text_sprites.clear();

        // header
        text_helpers::draw_centered(_text_generator,
                                    _title,
                                    screen_top + 12,
                                    _text_sprites);

        if (_current_cart_infos.has_value())
        {
            text_helpers::draw_centered_at(_text_generator,
                                           "Path: \"/dump/" + bn::string<32>(_current_cart_infos->cart_rom_infos.name) + ".gba\"",
                                           0,
                                           -48,
                                           _text_sprites);
        }

        // warning
        text_helpers::draw_centered_at(_text_generator,
                                       "DO NOT POWER OFF CONSOLE",
                                       10,
                                       60,
                                       _text_sprites);

        // process infos
        text_helpers::draw_centered_at(_text_generator,
                                       bn::to_string<32>(_current_process_infos.progress) + "%",
                                       85,
                                       27,
                                       _text_sprites);

        // status
        text_helpers::draw_centered_at(_text_generator,
                                       bn::string<32>("Status: ") + (_current_process_infos.status == process_status::WRITING ? "WRITING" : "READING"),
                                       0,
                                       15,
                                       _text_sprites);

        // elapsed time
        text_helpers::draw_centered_at(_text_generator,
                                       bn::string<32>("Elapsed: ") +
                                           bn::to_string<32>(_current_process_infos.elapsed_time.minutes) +
                                           ":" +
                                           bn::to_string<32>(_current_process_infos.elapsed_time.seconds),
                                       -bn::display::width() / 2 + 50,
                                       40,
                                       _text_sprites);

        // speed
        text_helpers::draw_centered_at(_text_generator,
                                       bn::string<32>("Speed: ") + bn::to_string<32>(_current_process_infos.speed) + "KiB/s",
                                       60,
                                       40,
                                       _text_sprites);

        for (auto &sprite : _sprites)
        {
            sprite.set_visible(true);
            sprite.set_bg_priority(0);
        }

        for (auto &sprite : _text_sprites)
            sprite.set_bg_priority(0);
    }

    void process_progress_scene::enter()
    {
        // Background
        bn::bg_tiles::set_allow_offset(false);
        _background.emplace(bn::regular_bg_item(
                                bn::regular_bg_tiles_items::tiles,
                                bn::regular_bg_tiles_items::tiles_palette,
                                openflash::process_progress_scene_bg_map_item)
                                .create_bg(0, 0));
        _background.value().set_top_left_position(0, 0);
        bn::regular_bg_map_ptr bg_map_ptr = _background.value().map();
        bg_map_ptr.reload_cells_ref();
        _background->set_priority(0);
        bn::bg_tiles::set_allow_offset(true);

        // cart infos
        _current_cart_infos = api::cart_api::instance().get_current_cart_infos();

        update_screen_infos();
    }

    void process_progress_scene::exit()
    {
        for (auto &sprite : _sprites)
            sprite.set_visible(false);
        _text_sprites.clear();
        _background.reset();
    }

    void process_progress_scene::update()
    {
        // get new infos form server side here (esp32), then update screen
        update_screen_infos();
    }

    void process_progress_scene::render()
    {
    }

    scene_type process_progress_scene::get_scene_type()
    {
        return _type;
    }

    void process_progress_scene::set_title(const bn::string_view &title)
    {
        _title = title;
    }
}
