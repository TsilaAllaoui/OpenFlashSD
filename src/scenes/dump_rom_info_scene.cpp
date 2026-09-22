#include "bn_keypad.h"
#include "bn_core.h"
#include "bn_display.h"
#include "bn_bg_tiles.h"
#include "bn_regular_bg_item.h"
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
#include "dump_rom_info_scene_bg.h"
#include "utilities/text_helpers.h"
#include "bn_regular_bg_tiles_items_tiles.h"
#include "common_variable_8x16_sprite_font.h"
#include "dump_rom_info_scene.h"

namespace openflash
{
    dump_rom_info_scene::dump_rom_info_scene()
        : _type(scene_type::DUMP_ROM_INFO),
          _background(),
          _pop_up_bg(),
          _text_generator(bn::sprite_text_generator(common::variable_8x16_sprite_font)),
          _sprites()
    {
        _sprites.emplace_back(bn::sprite_items::gbacart.create_sprite(screen_left + 30, screen_top + 120));
        _sprites.emplace_back(bn::sprite_items::arrow.create_sprite(screen_left + 46, screen_top + 120));
        _sprites.emplace_back(bn::sprite_items::save.create_sprite(screen_left + 12, screen_top + 12));
        _sprites.emplace_back(bn::sprite_items::sdcard.create_sprite(screen_left + 62, screen_top + 120));

        for (auto &sprite : _sprites)
            sprite.set_visible(false);
    }

    void dump_rom_info_scene::enter()
    {
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

        auto cart_infos_result = api::cart_api::instance().get_current_cart_infos();
        if (cart_infos_result.has_value())
        {
            _current_cart_infos = cart_infos_result.value();
            flash_context::instance().set_current_cart_infos(cart_infos_result.value());
        }

        auto current_rom_infos_in_cart = _current_cart_infos.cart_rom_infos;

        bn::string_view label = "Name:";
        text_helpers::draw_label_value(_text_generator,
                                       label,
                                       current_rom_infos_in_cart.name.empty() ? "Unkown name" : current_rom_infos_in_cart.name,
                                       dump_x_alignment,
                                       -dump_x_alignment,
                                       dump_scene_text_y_top,
                                       _text_sprites);

        label = "Game Code:";
        text_helpers::draw_label_value(_text_generator,
                                       label,
                                       current_rom_infos_in_cart.game_code.empty() ? "Unkown game code" : current_rom_infos_in_cart.game_code,
                                       dump_x_alignment,
                                       -dump_x_alignment,
                                       dump_scene_text_y_top + dump_scene_text_y_spacing,
                                       _text_sprites);

        label = "Marker code:";
        text_helpers::draw_label_value(_text_generator,
                                       label,
                                       current_rom_infos_in_cart.maker_code.empty() ? "Unkown marker code" : current_rom_infos_in_cart.maker_code,
                                       dump_x_alignment,
                                       -dump_x_alignment,
                                       dump_scene_text_y_top + dump_scene_text_y_spacing * 2,
                                       _text_sprites);

        label = "Save type:";
        text_helpers::draw_label_value(_text_generator,
                                       label,
                                       string_helpers::to_string(current_rom_infos_in_cart.savetype),
                                       dump_x_alignment,
                                       -dump_x_alignment,
                                       dump_scene_text_y_top + dump_scene_text_y_spacing * 3,
                                       _text_sprites);

        bn::string<max_file_patch_character> cart_name_text = "Cart: ";
        cart_name_text += cart_infos_result->name;
        text_helpers::draw_centered(_text_generator,
                                    cart_name_text,
                                    screen_top + 95,
                                    _text_sprites);

        text_helpers::draw_centered_at(_text_generator,
                                       "Dump to SD Card",
                                       20,
                                       screen_top + 120,
                                       _text_sprites);

        text_helpers::draw_centered(_text_generator,
                                    "A: Dump, B: Back, SLCT: Refresh",
                                    65, _text_sprites);

        for (auto &sprite : _sprites)
        {
            sprite.set_visible(true);
            sprite.set_bg_priority(0);
        }

        for (auto &sprite : _text_sprites)
            sprite.set_bg_priority(0);
    }

    void dump_rom_info_scene::exit()
    {
        for (auto &sprite : _sprites)
            sprite.set_visible(false);
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
                for (auto &sprite : _text_sprites)
                    sprite.set_bg_priority(1);
                for (auto &sprite : _sprites)
                    sprite.set_bg_priority(1);
                _background->set_priority(1);

                auto cart_infos = api::cart_api::instance().get_current_cart_infos();
                if (cart_infos.has_value())
                    flash_context::instance().set_current_cart_infos(cart_infos.value());

                for (auto &sprite : _text_sprites)
                    sprite.set_bg_priority(0);
                for (auto &sprite : _sprites)
                    sprite.set_bg_priority(0);
                _background->set_priority(0);
            }
            if (bn::keypad::a_pressed())
            {
                for (auto &sprite : _text_sprites)
                    sprite.set_bg_priority(1);
                for (auto &sprite : _sprites)
                    sprite.set_bg_priority(1);
                _background->set_priority(1);

                pop_up popup("Dump Cart?", true, true);
                popup.update();
                if (popup.get_confirmation_response())
                {
                    scene_state_machine::instance().request_scene_state(scene_type::PROCESS_PROGRESS);
                    return;
                }

                for (auto &sprite : _text_sprites)
                    sprite.set_bg_priority(0);
                for (auto &sprite : _sprites)
                    sprite.set_bg_priority(0);
                _background->set_priority(0);
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

    void dump_rom_info_scene::set_title(const bn::string_view& title)
    {
        _title = title;
    }
}
