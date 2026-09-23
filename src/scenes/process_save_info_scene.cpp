#include "bn_keypad.h"
#include "bn_display.h"
#include "bn_bg_tiles.h"
#include "bn_regular_bg_item.h"
#include "bn_sprite_items_save.h"
#include "bn_sprite_items_arrow.h"
#include "bn_regular_bg_map_ptr.h"
#include "bn_sprite_items_gbacart.h"
#include "bn_regular_bg_tiles_items_tiles.h"

#include "file_entry.h"
#include "api/cart_api.h"
#include "flash_context.h"
#include "string_helpers.h"
#include "scene_state_machine.h"
#include "dump_rom_info_scene_bg.h"
#include "api/process_info_api.h"
#include "utilities/text_helpers.h"
#include "process_save_info_scene.h"
#include "common_variable_8x8_sprite_font.h"
#include "common_variable_8x16_sprite_font.h"

namespace openflash
{
    process_save_info_scene::process_save_info_scene()
        : _type(scene_type::SAVE_PROCESS_SCREEN),
          _background(),
          _pop_up(),
          _text_generator_8x16(bn::sprite_text_generator(common::variable_8x16_sprite_font)),
          _text_generator_8x8(bn::sprite_text_generator(common::variable_8x8_sprite_font)),
          _sprites()
    {
        _sprites.emplace_back(bn::sprite_items::gbacart.create_sprite(screen_left + 30, screen_top + 120));
        _sprites.emplace_back(bn::sprite_items::arrow.create_sprite(screen_left + 46, screen_top + 120));
        _sprites.emplace_back(bn::sprite_items::save.create_sprite(screen_left + 12, screen_top + 12));
        _sprites.emplace_back(bn::sprite_items::save.create_sprite(screen_left + 62, screen_top + 120));

        for (auto &sprite : _sprites)
            sprite.set_visible(false);
    }

    void process_save_info_scene::enter()
    {
        auto current_save_process_infos = flash_context::instance().get_current_save_infos();

        if (!current_save_process_infos.has_value())
        {
            scene_state_machine::instance().request_scene_state(scene_type::MAIN_MENU);
            return;
        }

        auto cart_infos_result = api::cart_api::instance().get_current_cart_infos();

        if (!cart_infos_result.has_value())
        {
            scene_state_machine::instance().request_scene_state(scene_type::MAIN_MENU);
            return;
        }

        _current_cart_infos = cart_infos_result.value();
        flash_context::instance().set_current_cart_infos(cart_infos_result.value());

        _sprites[arrow_index].set_horizontal_flip(current_save_process_infos->status == process_status::WRITING);

        bn::string<max_character_count> type_text = current_save_process_infos->status == process_status::WRITING ? "Restore" : "Backup";
        type_text += " save";

        text_helpers::draw_centered(_text_generator_8x16,
                                    type_text,
                                    screen_top + 12,
                                    _text_sprites);

        bn::bg_tiles::set_allow_offset(false);
        _background.emplace(bn::regular_bg_item(
                                bn::regular_bg_tiles_items::tiles,
                                bn::regular_bg_tiles_items::tiles_palette,
                                openflash::dump_rom_info_scene_bg_map_item)
                                .create_bg(0, 0));
        _background->set_top_left_position(0, 0);
        bn::regular_bg_map_ptr bg_map_ptr = _background->map();
        bg_map_ptr.reload_cells_ref();
        _background->set_priority(0);
        bn::bg_tiles::set_allow_offset(true);

        auto current_rom_infos_in_cart = _current_cart_infos.cart_rom_infos;

        bn::string_view label = "Name:";
        text_helpers::draw_label_value(_text_generator_8x16,
                                       label,
                                       current_rom_infos_in_cart.name.empty() ? "Unkown name" : current_rom_infos_in_cart.name,
                                       process_save_x_alignment,
                                       -process_save_x_alignment,
                                       process_save_scene_text_y_top,
                                       _text_sprites);

        label = "Game Code:";
        text_helpers::draw_label_value(_text_generator_8x16,
                                       label,
                                       current_rom_infos_in_cart.game_code.empty() ? "Unkown game code" : current_rom_infos_in_cart.game_code,
                                       process_save_x_alignment,
                                       -process_save_x_alignment,
                                       process_save_scene_text_y_top + process_save_scene_text_y_spacing,
                                       _text_sprites);

        label = "Marker code:";
        text_helpers::draw_label_value(_text_generator_8x16,
                                       label,
                                       current_rom_infos_in_cart.maker_code.empty() ? "Unkown marker code" : current_rom_infos_in_cart.maker_code,
                                       process_save_x_alignment,
                                       -process_save_x_alignment,
                                       process_save_scene_text_y_top + process_save_scene_text_y_spacing * 2,
                                       _text_sprites);

        label = "Save type:";
        text_helpers::draw_label_value(_text_generator_8x16,
                                       label,
                                       string_helpers::to_string(current_rom_infos_in_cart.savetype),
                                       process_save_x_alignment,
                                       -process_save_x_alignment,
                                       process_save_scene_text_y_top + process_save_scene_text_y_spacing * 3,
                                       _text_sprites);

        bn::string<max_file_patch_character> cart_name_text = "Cart: ";
        cart_name_text += cart_infos_result->name;
        text_helpers::draw_centered(_text_generator_8x16,
                                    cart_name_text,
                                    screen_top + 95,
                                    _text_sprites);

        text_helpers::draw_centered_at(_text_generator_8x16,
                                       type_text,
                                       20,
                                       screen_top + 120,
                                       _text_sprites);

        bn::string<max_character_count> help_text = "A: ";
        help_text += bn::string<max_character_count>(current_save_process_infos->status == process_status::WRITING ? "Restore" : "Backup") + " B: Back SELECT: Refresh cart";
        text_helpers::draw_centered(_text_generator_8x8,
                                    help_text,
                                    65,
                                    _text_sprites);

        for (auto &sprite : _sprites)
        {
            sprite.set_visible(true);
            sprite.set_bg_priority(0);
        }

        for (auto &sprite : _text_sprites)
            sprite.set_bg_priority(0);
    }

    void process_save_info_scene::exit()
    {
        _pop_up.reset();

        for (auto &sprite : _sprites)
            sprite.set_visible(false);

        _text_sprites.clear();
        _background.reset();
    }

    void process_save_info_scene::update()
    {
        if (_pop_up)
        {
            _pop_up->update();

            if (_pop_up->is_open())
                return;

            bool confirmed = _pop_up->get_confirmation_response();
            _pop_up.reset();
            set_content_priority(0);

            if (confirmed)
            {
                api::process_info_api::instance().start(process_type::RESTORE_SAVE);
                scene_state_machine::instance().request_scene_state(scene_type::PROCESS_PROGRESS);
            }

            return;
        }

        if (bn::keypad::b_pressed())
        {
            flash_context::instance().set_current_file_filter(file_type::SAVE_FILE);
            scene_state_machine::instance().request_scene_state(scene_type::FILE_BROWSER);
            return;
        }

        if (bn::keypad::select_pressed())
        {
            auto cart_infos = api::cart_api::instance().get_current_cart_infos();

            if (cart_infos.has_value())
            {
                _current_cart_infos = cart_infos.value();
                flash_context::instance().set_current_cart_infos(cart_infos.value());
            }
        }

        if (bn::keypad::a_pressed())
        {
            auto current_save_process_infos = flash_context::instance().get_current_save_infos();

            if (!current_save_process_infos.has_value())
            {
                scene_state_machine::instance().request_scene_state(scene_type::MAIN_MENU);
                return;
            }

            bn::string<max_character_count> pop_up_header_text = current_save_process_infos->status == process_status::READING ? "Backup" : "Restore";
            pop_up_header_text += " Save?";

            set_content_priority(1);
            _pop_up.emplace(pop_up_header_text, true, true);
            _pop_up->render();
        }
    }

    void process_save_info_scene::set_content_priority(int priority)
    {
        if (_background)
            _background->set_priority(priority);

        for (auto &sprite : _text_sprites)
            sprite.set_bg_priority(priority);

        for (auto &sprite : _sprites)
            sprite.set_bg_priority(priority);
    }

    void process_save_info_scene::render()
    {
    }

    scene_type process_save_info_scene::get_scene_type()
    {
        return _type;
    }

    void process_save_info_scene::set_title(const bn::string_view &title)
    {
        _title = title;
    }
}
