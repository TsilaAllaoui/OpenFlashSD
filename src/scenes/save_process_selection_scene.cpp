#include "bn_keypad.h"
#include "bn_display.h"
#include "bn_bg_tiles.h"
#include "bn_regular_bg_item.h"
#include "bn_sprite_items_save.h"
#include "bn_sprite_items_arrow.h"
#include "bn_regular_bg_map_ptr.h"
#include "bn_sprite_items_gbacart.h"

#include "file_entry.h"
#include "api/cart_api.h"
#include "flash_context.h"
#include "color_helpers.h"
#include "big_selector_bg.h"
#include "scene_state_machine.h"
#include "utilities/text_helpers.h"
#include "save_process_selection_scene.h"
#include "bn_regular_bg_tiles_items_tiles.h"
#include "common_variable_8x8_sprite_font.h"
#include "common_variable_8x16_sprite_font.h"
#include "save_process_selection_scene_bg.h"

namespace openflash
{
    save_process_selection_scene::save_process_selection_scene()
        : _type(scene_type::SAVE_PROCESS_SELECTION_SCREEN), _background(), _big_selector_bg(), _pop_up(),
          _text_generator_8x16(bn::sprite_text_generator(common::variable_8x16_sprite_font)),
          _text_generator_8x8(bn::sprite_text_generator(common::variable_8x8_sprite_font)), _sprites(),
          _current_selector_index(0), _pending_cart_infos_request(false)
    {
        _sprites.emplace_back(bn::sprite_items::save.create_sprite(screen_left + 12, screen_top + 12));
        _sprites.emplace_back(bn::sprite_items::gbacart.create_sprite(screen_left + 40, screen_top + 60));
        _sprites.emplace_back(bn::sprite_items::arrow.create_sprite(screen_left + 56, screen_top + 60));
        _sprites.emplace_back(bn::sprite_items::save.create_sprite(screen_left + 72, screen_top + 60));
        _sprites.emplace_back(bn::sprite_items::gbacart.create_sprite(screen_left + 40, screen_top + 110));

        auto reverse_arrow_sprite = bn::sprite_items::arrow.create_sprite(screen_left + 56, screen_top + 110);
        reverse_arrow_sprite.set_horizontal_flip(true);
        _sprites.emplace_back(reverse_arrow_sprite);
        _sprites.emplace_back(bn::sprite_items::save.create_sprite(screen_left + 72, screen_top + 110));

        for (auto &sprite : _sprites)
            sprite.set_visible(false);
    }

    void save_process_selection_scene::enter()
    {
        text_helpers::draw_centered(_text_generator_8x16, "Save process", screen_top + 12, _text_sprites);

        bn::bg_tiles::set_allow_offset(false);
        _background.emplace(bn::regular_bg_item(bn::regular_bg_tiles_items::tiles,
                                                bn::regular_bg_tiles_items::tiles_palette,
                                                openflash::save_process_selection_scene_bg_map_item)
                                .create_bg(0, 0));
        _background->set_top_left_position(0, 0);
        bn::regular_bg_map_ptr bg_map_ptr = _background->map();
        bg_map_ptr.reload_cells_ref();
        _background->set_priority(1);
        bn::bg_tiles::set_allow_offset(true);

        bn::bg_tiles::set_allow_offset(false);
        _big_selector_bg.emplace(bn::regular_bg_item(bn::regular_bg_tiles_items::tiles,
                                                     bn::regular_bg_tiles_items::tiles_palette,
                                                     openflash::big_selector_bg_map_item)
                                     .create_bg(0, 0));
        _big_selector_bg->set_top_left_position(0, 0);
        bn::regular_bg_map_ptr selector_bg_map_ptr = _big_selector_bg->map();
        selector_bg_map_ptr.reload_cells_ref();
        _big_selector_bg->set_priority(0);
        bn::bg_tiles::set_allow_offset(true);

        text_helpers::draw_centered_at(_text_generator_8x8, "Backup save to SD Card", 25, -20, _text_sprites);

        text_helpers::draw_centered_at(_text_generator_8x8, "Restore from SD Card", 25, 30, _text_sprites);

        text_helpers::draw_centered(_text_generator_8x8, "A: Choose  B: Back  SELECT: Refresh Cart", 65, _text_sprites);

        for (auto &sprite : _sprites)
        {
            sprite.set_visible(true);
            sprite.set_bg_priority(1);
        }

        for (auto &sprite : _text_sprites)
            sprite.set_bg_priority(1);
    }

    void save_process_selection_scene::exit()
    {
        _pop_up.reset();
        _pending_cart_infos_request = false;

        for (auto &sprite : _sprites)
            sprite.set_visible(false);

        _text_sprites.clear();
        _background.reset();
        _big_selector_bg.reset();
    }

    void save_process_selection_scene::update()
    {
        if (_pending_cart_infos_request)
        {
            auto &cart_api = api::cart_api::instance();
            cart_api.update();

            if (!cart_api.response_available())
                return;

            const auto &cart_infos = cart_api.get_cart_infos_response();
            flash_context::instance().set_current_cart_infos(cart_infos);
            _current_cart_infos = cart_infos;
            _pending_cart_infos_request = false;
            _pop_up.reset();
            _big_selector_bg->set_priority(0);
            return;
        }

        if (_pop_up)
        {
            _pop_up->update();

            auto confirmation_status = _pop_up->get_confirmation_response();

            if (confirmation_status == confirmation_request_status::NEGATIVE)
            {
                _pop_up.reset();
                _big_selector_bg->set_priority(0);
                return;
            }

            else if (confirmation_status == confirmation_request_status::POSITIVE)
            {
                if (_current_selector_index == 1)
                {
                    flash_context::instance().set_current_file_filter(file_type::SAVE_FILE);
                    scene_state_machine::instance().request_scene_state(scene_type::FILE_BROWSER);
                    _pop_up.reset();
                    return;
                }
                else
                {
                    scene_state_machine::instance().request_process_progress(process_type::BACKUP_SAVE);
                    _pop_up.reset();
                    return;
                }
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
            _pending_cart_infos_request = true;
            api::cart_api::instance().request_cart_infos();
            _big_selector_bg->set_priority(1);
            _pop_up.emplace("Getting cart infos...", false);
            _pop_up->render();
            return;
        }

        if (bn::keypad::a_pressed())
        {
            bn::string<max_character_count> popup_text_header = _current_selector_index == 0 ? "Backup" : "Restore";
            popup_text_header += " Save?";

            _big_selector_bg->set_priority(1);
            _pop_up.emplace(popup_text_header, true, true);
            _pop_up->render();
            return;
        }

        if (bn::keypad::up_pressed() && _current_selector_index == 1)
        {
            _current_selector_index = 0;
            _big_selector_bg->set_y(48);
        }

        if (bn::keypad::down_pressed() && _current_selector_index == 0)
        {
            _current_selector_index = 1;
            _big_selector_bg->set_y(96);
        }
    }

    void save_process_selection_scene::render()
    {
        if (_pop_up)
            return;

        static bn::color first_color(0, 17, 17);
        static bn::color second_color(0, 25, 25);
        static int frames = 0;
        static bool change = false;

        frames++;

        if (frames >= 25)
        {
            frames = 0;

            if (!change)
                color_helpers::replace_bg_color(*_big_selector_bg, first_color, second_color);
            else
                color_helpers::replace_bg_color(*_big_selector_bg, second_color, first_color);

            change = !change;
        }
    }

    scene_type save_process_selection_scene::get_scene_type()
    {
        return _type;
    }

    void save_process_selection_scene::set_title(const bn::string_view &title)
    {
        _title = title;
    }
} // namespace openflash
