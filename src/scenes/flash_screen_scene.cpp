#include "bn_keypad.h"
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
          _pop_up(),
          _text_generator_8x16(bn::sprite_text_generator(common::variable_8x16_sprite_font)),
          _text_generator_8x8(bn::sprite_text_generator(common::variable_8x8_sprite_font)),
          _gbacart_sprite(bn::sprite_items::gbacart.create_sprite(screen_left + 12, screen_top + 12)),
          _request_status(request_status::IDLE),
          _popup()
    {
        _gbacart_sprite.set_visible(false);

        set_content_priority(1);
    }

    void flash_screen_scene::render_cart_infos()
    {
        _text_sprites.clear();

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

        if (!cart_infos)
            BN_ERROR("Cart infos is nullopt");

        bn::string<max_character_count> cart_infos_text = "Cart: ";
        cart_infos_text += cart_infos->name;
        text_helpers::draw_centered(_text_generator_8x16, cart_infos_text, 30, _text_sprites);

        text_helpers::draw_centered(_text_generator_8x8,
                                    "A: Flash  B: Back  SELECT: Refresh Cart",
                                    65, _text_sprites);


        set_content_priority(1);
    }

    void flash_screen_scene::enter()
    {
        _gbacart_sprite.set_visible(true);
        _gbacart_sprite.set_bg_priority(1);

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
        bn::bg_tiles::set_allow_offset(true);

        render_cart_infos();
    }

    void flash_screen_scene::exit()
    {
        _pop_up.reset();
        _popup.reset();
        _request_status = request_status::IDLE;
        _gbacart_sprite.set_visible(false);
        _text_sprites.clear();
        _background.reset();
    }

    void flash_screen_scene::update()
    {
        if (_request_status == request_status::PENDING)
        {
            auto &cart_api = api::cart_api::instance();
            cart_api.update();

            if (!cart_api.response_available())
                return;

            flash_context::instance().set_current_cart_infos(cart_api.get_cart_infos_response());
            _popup.reset();
            _request_status = request_status::RECENTLY_CHANGED;
            return;
        }

        if (_request_status == request_status::RECENTLY_CHANGED)
        {
            _request_status = request_status::IDLE;
            render_cart_infos();
            return;
        }

        if (_pop_up)
        {
            _pop_up->update();

            auto confirmation_status = _pop_up->get_confirmation_response();

            if (confirmation_status == confirmation_request_status::POSITIVE)
            {
                flash_context::instance().set_requested_process_type(process_type::FLASHING);
                _pop_up.reset();
                scene_state_machine::instance().request_scene_state(scene_type::PROCESS_PROGRESS);
            }
            else if (confirmation_status == confirmation_request_status::NEGATIVE)
            {
                set_content_priority(1);
                _pop_up.reset();
            }

            return;
        }

        if (bn::keypad::a_pressed())
        {
            set_content_priority(1);
            _pop_up.emplace("Flash Rom?", true, true);
            _pop_up->render();
            return;
        }

        if (bn::keypad::b_pressed())
        {
            scene_state_machine::instance().request_scene_state(scene_type::FILE_BROWSER);
            return;
        }

        if (bn::keypad::select_pressed())
        {
            _request_status = request_status::PENDING;
            api::cart_api::instance().request_cart_infos();
            _popup.emplace("Getting cart infos...", false);
            _popup->render();
            return;
        }
    }

    void flash_screen_scene::set_content_priority(int priority)
    {
        if (_background)
            _background->set_priority(priority);

        _gbacart_sprite.set_bg_priority(priority);

        for (auto &sprite : _text_sprites)
            sprite.set_bg_priority(priority);
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
