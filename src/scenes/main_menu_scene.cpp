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

#include "cart_api.h"
#include "flash_context.h"
#include "utilities/pop_up.h"
#include "utilities/text_helpers.h"

namespace openflash
{
    main_menu_bg_scene::main_menu_bg_scene()
        : _type(scene_type::MAIN_MENU),
          _background(),
          _selector(),
          _text_generator(bn::sprite_text_generator(common::variable_8x16_sprite_font)),
          _current_menu_index(0)
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

        // background
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
                                    "A: Choose, SELECT: Refresh cart",
                                    65,
                                    _text_sprites);

        // selector
        _selector.render();
    }

    void main_menu_bg_scene::exit()
    {
        _selector.dismiss();
        _text_sprites.clear();
        _background.reset();
    }

    void main_menu_bg_scene::update()
    {
        if (bn::keypad::right_pressed())
        {
            if (_current_menu_index < 3)
                _current_menu_index++;
            _selector.update_position(_current_menu_index);
        }
        if (bn::keypad::left_pressed())
        {
            if (_current_menu_index > 0)
                _current_menu_index--;
            _selector.update_position(_current_menu_index);
        }
        if (bn::keypad::a_pressed())
        {
            if (_current_menu_index == 0)
            {
                scene_state_machine::instance().request_scene_state(scene_type::FILE_BROWSER);
            }
        }
        if (bn::keypad::select_pressed())
        {
            auto current_cart_infos = api::cart_api::instance().get_current_cart_infos();
            if (current_cart_infos.has_value())
            {
                flash_context::instance().set_current_cart_infos(current_cart_infos.value());
            }
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
