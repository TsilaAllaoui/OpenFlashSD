#include "flash_screen_scene.h"

#include "bn_keypad.h"
#include "bn_core.h"
#include "bn_display.h"
#include "bn_bg_tiles.h"
#include "bn_regular_bg_item.h"
#include "bn_regular_bg_map_ptr.h"

#include "flash_screen.h"
#include "file_entry.h"
#include "scene_state_machine.h"
#include "bn_regular_bg_tiles_items_tiles.h"
#include "common_variable_8x16_sprite_font.h"

#include "utilities/pop_up.h"

namespace openflash
{
    flash_screen_scene::flash_screen_scene()
        : _type(scene_type::FLASH_SCREEN),
          _background(),
          _pop_up_bg(),
          _text_generator(bn::sprite_text_generator(common::variable_8x16_sprite_font))
    {
    }

    void flash_screen_scene::enter()
    {
        _text_generator.set_left_alignment();

        // header
        _text_generator.generate(
            screen_left + 80,
            screen_top + 12,
            "ROM Information",
            _text_sprites);

        // Background
        bn::bg_tiles::set_allow_offset(false);
        _background.emplace(bn::regular_bg_item(
                                bn::regular_bg_tiles_items::tiles,
                                bn::regular_bg_tiles_items::tiles_palette,
                                openflash::flash_screen_map_item)
                                .create_bg(0, 0));
        _background.value().set_top_left_position(0, 0);
        bn::regular_bg_map_ptr bg_map_ptr = _background.value().map();
        bg_map_ptr.reload_cells_ref();
        _background->set_priority(0);
        bn::bg_tiles::set_allow_offset(true);

        _text_generator.generate(
            -(bn::display::width() / 2) + 65, 40,
            "PROGRAM CARTRIDGE",
            _text_sprites);

        _text_generator.generate(
            -(bn::display::width() / 2) + 40, 65,
            "A: Program       B: Back",
            _text_sprites);

        for (auto &sprite : _text_sprites)
            sprite.set_bg_priority(0);
    }

    void flash_screen_scene::exit()
    {
        _text_sprites.clear();
        _background.reset();
    }

    void flash_screen_scene::update()
    {
        while (true)
        {
            bn::core::update();
            if (bn::keypad::b_pressed())
            {
                scene_state_machine::instance().set_current_scene_state(scene_type::FILE_BROWSER);
                break;
            }
        }
    }

    void flash_screen_scene::render()
    {
    }

    scene_type flash_screen_scene::get_scene_type()
    {
        return _type;
    }
}
