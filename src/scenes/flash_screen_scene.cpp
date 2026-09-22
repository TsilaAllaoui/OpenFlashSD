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
#include "utilities/text_helpers.h"
#include "bn_regular_bg_tiles_items_tiles.h"
#include "common_variable_8x16_sprite_font.h"
#include "common_variable_8x8_sprite_font.h"
#include "flash_screen_scene.h"

namespace openflash
{
    flash_screen_scene::flash_screen_scene()
        : _type(scene_type::FLASH_SCREEN),
          _background(),
          _pop_up_bg(),
          _text_generator_8x16(bn::sprite_text_generator(common::variable_8x16_sprite_font)),
          _text_generator_8x8(bn::sprite_text_generator(common::variable_8x8_sprite_font)),
          _gbacart_sprite(bn::sprite_items::gbacart.create_sprite(screen_left + 12, screen_top + 12))
    {
        _gbacart_sprite.set_visible(false);
    }

    void flash_screen_scene::enter()
    {
        _gbacart_sprite.set_visible(true);
        _gbacart_sprite.set_bg_priority(0);

        // header
        text_helpers::draw_centered(_text_generator_8x16, "ROM Information", screen_top + 12, _text_sprites);

        // Background
        bn::bg_tiles::set_allow_offset(false);
        _background.emplace(bn::regular_bg_item(
                                bn::regular_bg_tiles_items::tiles,
                                bn::regular_bg_tiles_items::tiles_palette,
                                openflash::flash_screen_bg_map_item)
                                .create_bg(0, 0));
        _background.value().set_top_left_position(0, 0);
        bn::regular_bg_map_ptr bg_map_ptr = _background.value().map();
        bg_map_ptr.reload_cells_ref();
        _background->set_priority(0);
        bn::bg_tiles::set_allow_offset(true);

        auto rom_infos = flash_context::instance().get_current_rom_infos();

        if (rom_infos.has_value())
        {
            bn::string_view label = "Name:";
            text_helpers::draw_label_value(_text_generator_8x16,
                                           label,
                                           rom_infos->name.empty() ? "Unkown name" : rom_infos->name,
                                           dump_x_alignment,
                                           -dump_x_alignment,
                                           flash_scene_text_y_top,
                                           _text_sprites);

            label = "Game Code:";
            text_helpers::draw_label_value(_text_generator_8x16,
                                           label,
                                           rom_infos->game_code.empty() ? "Unkown game code" : rom_infos->game_code,
                                           dump_x_alignment,
                                           -dump_x_alignment,
                                           flash_scene_text_y_top + text_spacing_y,
                                           _text_sprites);

            label = "Marker code:";
            text_helpers::draw_label_value(_text_generator_8x16,
                                           label,
                                           rom_infos->maker_code.empty() ? "Unkown marker code" : rom_infos->maker_code,
                                           dump_x_alignment,
                                           -dump_x_alignment,
                                           flash_scene_text_y_top + text_spacing_y * 2,
                                           _text_sprites);

            label = "Save type:";
            text_helpers::draw_label_value(_text_generator_8x16,
                                           label,
                                           string_helpers::to_string(rom_infos->savetype),
                                           dump_x_alignment,
                                           -dump_x_alignment,
                                           flash_scene_text_y_top + text_spacing_y * 3,
                                           _text_sprites);
        }

        auto cart_infos = flash_context::instance().get_current_cart_infos();

        if (cart_infos.has_value())
        {
            bn::string<max_character_count> cart_infos_text = "Cart: ";
            cart_infos_text += cart_infos->name;
            text_helpers::draw_centered(_text_generator_8x16, cart_infos_text, 30, _text_sprites);
        }

        text_helpers::draw_centered(_text_generator_8x8,
                                    "A: Flash  B: Back  SELECT: Refresh Cart",
                                    65, _text_sprites);

        for (auto &sprite : _text_sprites)
            sprite.set_bg_priority(0);
    }

    void flash_screen_scene::exit()
    {
        _gbacart_sprite.set_visible(false);
        _text_sprites.clear();
        _background.reset();
    }

    void flash_screen_scene::update()
    {
        bn::core::update();
        if (bn::keypad::a_pressed())
        {
            for (auto &sprite : _text_sprites)
                sprite.set_bg_priority(1);

            _gbacart_sprite.set_bg_priority(1);
            _background->set_priority(1);

            pop_up popup("Flash Rom?", true, true);
            popup.update();
            if (popup.get_confirmation_response())
            {
                scene_state_machine::instance().request_scene_state(scene_type::PROCESS_PROGRESS);
                return;
            }

            for (auto &sprite : _text_sprites)
                sprite.set_bg_priority(0);
            _gbacart_sprite.set_bg_priority(0);
            _background->set_priority(0);
        }
        else if (bn::keypad::b_pressed())
        {
            scene_state_machine::instance().request_scene_state(scene_type::FILE_BROWSER);
        }
        else if (bn::keypad::select_pressed())
        {
            for (auto &sprite : _text_sprites)
                sprite.set_bg_priority(1);
            _gbacart_sprite.set_bg_priority(1);
            _background->set_priority(1);

            auto cart_infos = api::cart_api::instance().get_current_cart_infos();
            if (cart_infos.has_value())
                flash_context::instance().set_current_cart_infos(cart_infos.value());

            for (auto &sprite : _text_sprites)
                sprite.set_bg_priority(0);
            _gbacart_sprite.set_bg_priority(0);
            _background->set_priority(0);
        }
    }

    void flash_screen_scene::render()
    {
    }

    scene_type flash_screen_scene::get_scene_type()
    {
        return _type;
    }

    void flash_screen_scene::set_title(const bn::string_view &title)
    {
        _title = title;
    }
}
