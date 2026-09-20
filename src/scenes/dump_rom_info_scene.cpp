#include "bn_keypad.h"
#include "bn_core.h"
#include "bn_display.h"
#include "bn_bg_tiles.h"
#include "bn_regular_bg_item.h"
#include "bn_regular_bg_map_ptr.h"
#include "bn_sprite_items_gbacart.h"

#include "file_entry.h"
#include "api/cart_api.h"
#include "flash_context.h"
#include "string_helpers.h"
#include "flash_screen_bg.h"
#include "utilities/pop_up.h"
#include "scene_state_machine.h"
#include "dump_rom_info_scene_bg.h"
#include "utilities/text_helpers.h"
#include "bn_regular_bg_tiles_items_tiles.h"
#include "common_variable_8x16_sprite_font.h"
#include "dump_rom_info_scene.h"

namespace openflash
{
    dump_rom_info_scene::dump_rom_info_scene()
        : _type(scene_type::FLASH_SCREEN),
          _background(),
          _pop_up_bg(),
          _text_generator(bn::sprite_text_generator(common::variable_8x16_sprite_font)),
          _gbacart_sprite(bn::sprite_items::gbacart.create_sprite(screen_left + 12, screen_top + 12))
    {
        _gbacart_sprite.set_visible(false);
    }

    void dump_rom_info_scene::enter()
    {
        _gbacart_sprite.set_visible(true);
        _gbacart_sprite.set_bg_priority(0);

        // header
        text_helpers::draw_centered(_text_generator, "ROM Information", screen_top + 12, _text_sprites);

        // Background
        bn::bg_tiles::set_allow_offset(false);
        _background.emplace(bn::regular_bg_item(
                                bn::regular_bg_tiles_items::tiles,
                                bn::regular_bg_tiles_items::tiles_palette,
                                openflash::dump_rom_info_scene_bg_map_item)
                                .create_bg(0, 0));
        _background.value().set_top_left_position(0, 0);
        bn::regular_bg_map_ptr bg_map_ptr = _background.value().map();
        bg_map_ptr.reload_cells_ref();
        _background->set_priority(0);
        bn::bg_tiles::set_allow_offset(true);

        auto cart_infos = flash_context::instance().get_current_cart_infos();

        if (cart_infos.has_value())
        {
            auto current_rom_infos_in_cart = cart_infos->cart_rom_infos;

            bn::string_view label = "Name: ";
            text_helpers::draw_label_value(_text_generator,
                                           label,
                                           current_rom_infos_in_cart.name.empty() ? "Unkown name" : current_rom_infos_in_cart.name,
                                           -bn::display::width() / 2 + 20,
                                           bn::display::width() / 6,
                                           dump_scene_text_y_top,
                                           _text_sprites);

            label = "Game Code: ";
            text_helpers::draw_label_value(_text_generator,
                                           label,
                                           current_rom_infos_in_cart.game_code.empty() ? "Unkown game code" : current_rom_infos_in_cart.game_code,
                                           -bn::display::width() / 2 + 20,
                                           bn::display::width() / 6,
                                           dump_scene_text_y_top + dump_scene_text_y_spacing,
                                           _text_sprites);

            label = "Marker code";
            text_helpers::draw_label_value(_text_generator,
                                           label,
                                           current_rom_infos_in_cart.maker_code.empty() ? "Unkown marker code" : current_rom_infos_in_cart.maker_code,
                                           -bn::display::width() / 2 + 20,
                                           bn::display::width() / 6,
                                           dump_scene_text_y_top + dump_scene_text_y_spacing * 2,
                                           _text_sprites);

            label = "Save type";
            text_helpers::draw_label_value(_text_generator,
                                           label,
                                           string_helpers::to_string(current_rom_infos_in_cart.savetype),
                                           -bn::display::width() / 2 + 20,
                                           bn::display::width() / 6,
                                           dump_scene_text_y_top + dump_scene_text_y_spacing * 3,
                                           _text_sprites);
            
                                           text_helpers::draw_centered(_text_generator, cart_infos->name, 40, _text_sprites);
        }

        text_helpers::draw_centered(_text_generator,
                                    "A: Dump, B: Back, SLCT: Refresh",
                                    65, _text_sprites);

        for (auto &sprite : _text_sprites)
            sprite.set_bg_priority(0);
    }

    void dump_rom_info_scene::exit()
    {
        _gbacart_sprite.set_visible(false);
        _text_sprites.clear();
        _background.reset();
    }

    void dump_rom_info_scene::update()
    {
        while (true)
        {
            bn::core::update();
            if (bn::keypad::b_pressed())
            {
                scene_state_machine::instance().request_scene_state(scene_type::MAIN_MENU);
                break;
            }
            if (bn::keypad::select_pressed())
            {
                auto cart_infos = api::cart_api::instance().get_current_cart_infos();
                if (cart_infos.has_value())
                    flash_context::instance().set_current_cart_infos(cart_infos.value());
            }
        }
    }

    void dump_rom_info_scene::render()
    {
    }

    scene_type dump_rom_info_scene::get_scene_type()
    {
        return _type;
    }
}
