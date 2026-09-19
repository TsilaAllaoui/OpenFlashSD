#include "bn_keypad.h"
#include "bn_core.h"
#include "bn_display.h"
#include "bn_bg_tiles.h"
#include "bn_regular_bg_item.h"
#include "bn_regular_bg_map_ptr.h"

#include "main_menu_bg.h"
#include "pop_up_bg.h"
#include "file_entry.h"
#include "scene_state_machine.h"
#include "bn_regular_bg_tiles_items_tiles.h"
#include "common_variable_8x16_sprite_font.h"

#include "utilities/pop_up.h"
#include "utilities/text_helpers.h"

namespace openflash
{
    main_menu_bg_scene::main_menu_bg_scene()
        : _type(scene_type::MAIN_MENU),
          _background(),
          _pop_up_bg(),
          _text_generator(bn::sprite_text_generator(common::variable_8x16_sprite_font))
    {
    }

    void main_menu_bg_scene::enter()
    {
        _text_generator.set_left_alignment();

        // header
        text_helpers::draw_centered(_text_generator,
                                    "OpenFlashSD",
                                    screen_top + 12,
                                    _text_sprites);

        // Background
        bn::bg_tiles::set_allow_offset(false);
        _background.emplace(bn::regular_bg_item(
                                bn::regular_bg_tiles_items::tiles,
                                bn::regular_bg_tiles_items::tiles_palette,
                                openflash::main_menu_bg_map_item)
                                .create_bg(0, 0));
        _background.value().set_top_left_position(0, 0);
        bn::regular_bg_map_ptr bg_map_ptr = _background.value().map();
        bg_map_ptr.reload_cells_ref();
        bn::bg_tiles::set_allow_offset(true);

        text_helpers::draw_centered(_text_generator,
                                    "START: show file browser",
                                    0,
                                    _text_sprites);

        text_helpers::draw_centered(_text_generator,
                                    "SELECT: Refresh cart",
                                    50,
                                    _text_sprites);

        
    }

    void main_menu_bg_scene::exit()
    {
        _text_sprites.clear();
        _background.reset();
    }

    void main_menu_bg_scene::update()
    {
        if (bn::keypad::start_pressed())
        {
            scene_state_machine::instance().request_scene_state(scene_type::FILE_BROWSER);
        }
    }

    void main_menu_bg_scene::render()
    {
    }

    scene_type main_menu_bg_scene::get_scene_type()
    {
        return _type;
    }
}
