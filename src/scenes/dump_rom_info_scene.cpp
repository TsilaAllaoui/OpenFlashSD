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
#include "common_variable_8x8_sprite_font.h"
#include "common_variable_8x16_sprite_font.h"
#include "dump_rom_info_scene.h"

namespace openflash
{
    dump_rom_info_scene::dump_rom_info_scene()
        : _type(scene_type::DUMP_ROM_INFO), _background(), _confirmation_pop_up(),
          _text_generator_8x16(bn::sprite_text_generator(common::variable_8x16_sprite_font)),
          _text_generator_8x8(bn::sprite_text_generator(common::variable_8x8_sprite_font)), _sprites(),
          _request_status(request_status::IDLE), _popup()
    {
        _sprites.emplace_back(bn::sprite_items::gbacart.create_sprite(screen_left + 30, screen_top + 120));
        _sprites.emplace_back(bn::sprite_items::arrow.create_sprite(screen_left + 46, screen_top + 120));
        _sprites.emplace_back(bn::sprite_items::save.create_sprite(screen_left + 12, screen_top + 12));
        _sprites.emplace_back(bn::sprite_items::sdcard.create_sprite(screen_left + 62, screen_top + 120));

        for (auto &sprite : _sprites)
            sprite.set_visible(false);

        set_content_priority(1);
    }

    void dump_rom_info_scene::set_content_priority(int priority)
    {
        if (_background)
            _background->set_priority(priority);

        for (auto &sprite : _sprites)
            sprite.set_bg_priority(priority);

        _text_generator_8x16.set_bg_priority(priority);
        _text_generator_8x8.set_bg_priority(priority);

        for (auto &sprite : _text_sprites)
            sprite.set_bg_priority(priority);
    }

    void dump_rom_info_scene::render_cart_infos()
    {
        auto cart_infos_result = flash_context::instance().get_current_cart_infos();
        if (!cart_infos_result)
            BN_ERROR("Cart infos is nullopt");

        _text_sprites.clear();

        _current_cart_infos = cart_infos_result.value();
        flash_context::instance().set_current_cart_infos(cart_infos_result.value());

        auto current_rom_infos_in_cart = _current_cart_infos.cart_rom_infos;

        bn::string_view label = "Name:";
        text_helpers::draw_label_value(_text_generator_8x16,
                                       label,
                                       current_rom_infos_in_cart.name.empty() ? "Unkown name"
                                                                              : current_rom_infos_in_cart.name,
                                       dump_x_alignment,
                                       -dump_x_alignment,
                                       dump_scene_text_y_top,
                                       _text_sprites);

        label = "Game Code:";
        text_helpers::draw_label_value(_text_generator_8x16,
                                       label,
                                       current_rom_infos_in_cart.game_code.empty()
                                           ? "Unkown game code"
                                           : current_rom_infos_in_cart.game_code,
                                       dump_x_alignment,
                                       -dump_x_alignment,
                                       dump_scene_text_y_top + dump_scene_text_y_spacing,
                                       _text_sprites);

        label = "Marker code:";
        text_helpers::draw_label_value(_text_generator_8x16,
                                       label,
                                       current_rom_infos_in_cart.maker_code.empty()
                                           ? "Unkown marker code"
                                           : current_rom_infos_in_cart.maker_code,
                                       dump_x_alignment,
                                       -dump_x_alignment,
                                       dump_scene_text_y_top + dump_scene_text_y_spacing * 2,
                                       _text_sprites);

        label = "Save type:";
        text_helpers::draw_label_value(_text_generator_8x16,
                                       label,
                                       string_helpers::to_string(current_rom_infos_in_cart.savetype),
                                       dump_x_alignment,
                                       -dump_x_alignment,
                                       dump_scene_text_y_top + dump_scene_text_y_spacing * 3,
                                       _text_sprites);

        bn::string<max_file_patch_character> cart_name_text = "Cart: ";
        cart_name_text += cart_infos_result->name;
        text_helpers::draw_centered(_text_generator_8x16, cart_name_text, screen_top + 95, _text_sprites);

        text_helpers::draw_centered_at(_text_generator_8x16, "Dump to SD Card", 20, screen_top + 120, _text_sprites);

        text_helpers::draw_centered(_text_generator_8x8, "A: Dump  B: Back  SELECT: Refresh Cart", 65, _text_sprites);

        for (auto &sprite : _sprites)
        {
            sprite.set_visible(true);
        }

        set_content_priority(1);
    }

    void dump_rom_info_scene::enter()
    {
        // header
        text_helpers::draw_centered(_text_generator_8x16, "ROM Information", screen_top + 12, _text_sprites);

        // Background
        bn::bg_tiles::set_allow_offset(false);
        _background.emplace(bn::regular_bg_item(bn::regular_bg_tiles_items::tiles,
                                                bn::regular_bg_tiles_items::tiles_palette,
                                                openflash::dump_rom_info_scene_bg_map_item)
                                .create_bg(0, 0));
        _background.value().set_top_left_position(0, 0);
        _background->set_priority(1);
        bn::regular_bg_map_ptr bg_map_ptr = _background.value().map();
        bg_map_ptr.reload_cells_ref();
        bn::bg_tiles::set_allow_offset(true);

        // request cart infos
        _request_status = request_status::PENDING;
        api::cart_api::instance().request_cart_infos();
        _popup.emplace("Getting cart infos...", false);
        _popup->render();
    }

    void dump_rom_info_scene::exit()
    {
        _confirmation_pop_up.reset();
        _popup.reset();

        for (auto &sprite : _sprites)
            sprite.set_visible(false);

        _text_sprites.clear();
        _background.reset();
    }

    void dump_rom_info_scene::update()
    {
        if (_request_status == request_status::PENDING)
        {
            api::cart_api::instance().update();

            if (api::cart_api::instance().response_available())
            {
                _popup.reset();
                _request_status = request_status::RECENTLY_CHANGED;
                auto current_cart_infos = api::cart_api::instance().get_cart_infos_response();
                flash_context::instance().set_current_cart_infos(current_cart_infos);
            }

            return;
        }

        if (_request_status == request_status::RECENTLY_CHANGED)
        {
            _request_status = request_status::IDLE;

            render_cart_infos();
        }

        if (_confirmation_pop_up)
        {
            _confirmation_pop_up->update();

            auto confirmation_status = _confirmation_pop_up->get_confirmation_response();

            if (confirmation_status == confirmation_request_status::POSITIVE)
            {
                scene_state_machine::instance().request_process_progress(process_type::DUMPING);
                set_content_priority(1);
                _confirmation_pop_up.reset();
            }
            else if (confirmation_status == confirmation_request_status::NEGATIVE)
            {
                _confirmation_pop_up.reset();
            }

            return;
        }

        if (bn::keypad::b_pressed())
        {
            scene_state_machine::instance().request_scene_state(scene_type::MAIN_MENU);
            return;
        }

        if (bn::keypad::select_pressed())
        {
            _request_status = request_status::PENDING;
            api::cart_api::instance().request_cart_infos();
            _popup.emplace("Getting cart infos...", false);
            _popup->render();
        }

        if (bn::keypad::a_pressed())
        {
            set_content_priority(1);
            _confirmation_pop_up.emplace("Dump Cart?", true, true);
            _confirmation_pop_up->render();
        }
    }

    void dump_rom_info_scene::render()
    {
    }

    scene_type dump_rom_info_scene::get_scene_type()
    {
        return _type;
    }

    void dump_rom_info_scene::set_title(const bn::string_view &title)
    {
        _title = title;
    }
} // namespace openflash
