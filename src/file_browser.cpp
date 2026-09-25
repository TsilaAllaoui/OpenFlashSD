#include "bn_string.h"
#include "bn_keypad.h"
#include "bn_algorithm.h"
#include "bn_sprite_items_file.h"
#include "bn_sprite_items_save.h"
#include "bn_sprite_items_folder.h"
#include "bn_sprite_items_cursor.h"
#include "bn_sprite_items_gbacart.h"

#include "api/cart_api.h"
#include "file_browser.h"
#include "flash_context.h"
#include "api/rom_info_api.h"
#include "api/save_info_api.h"
#include "api/filesystem_api.h"
#include "scene_state_machine.h"
#include "utilities/text_helpers.h"
#include "common_variable_8x16_sprite_font.h"

constexpr int max_file_character_length = 22;

namespace openflash
{
    file_browser::file_browser(bn::optional<file_type> file_filter)
        : _text_generator(common::variable_8x16_sprite_font),
          _text_sprites(bn::vector<bn::sprite_ptr, max_file_count>()),
          _cursor_sprite(bn::sprite_items::cursor.create_sprite(screen_left + 12, file_y)),
          _current_file_indices(),
          _icons(bn::vector<bn::sprite_ptr, max_file_count_pagination>()),
          _pop_up(),
          _browser_state(),
          _restore_history(true),
          _file_filter(file_filter),
          _pending_cart_infos_request(false)
    {
        _browser_state = {
            .current_folder_id = -1,
            .current_file_index = 0,
            .need_update = true};

        _text_generator.set_bg_priority(1);
        _cursor_sprite.set_bg_priority(1);
        _cursor_sprite.set_visible(false);
    }

    const file_entry &file_browser::current_file(int index) const
    {
        const auto &files = api::filesystem_api::instance().get_files_response();
        return files[_current_file_indices[index]];
    }

    void file_browser::request_files()
    {
        _current_file_indices.clear();
        _history.clear();

        _browser_state.current_folder_id = -1;
        _browser_state.current_file_index = 0;
        _browser_state.need_update = true;

        _cursor_sprite.set_visible(false);

        api::filesystem_api::instance().request_files(_file_filter);
    }

    bool file_browser::update_file_loading()
    {
        auto &filesystem = api::filesystem_api::instance();
        filesystem.update();

        if (!filesystem.response_available())
            return false;

        update_current_files();
        _cursor_sprite.set_visible(!_current_file_indices.empty());
        _browser_state.need_update = true;

        return true;
    }

    bool file_browser::render_file_list()
    {
        if (!_browser_state.need_update)
            return false;

        _browser_state.need_update = false;

        _text_sprites.clear();
        _icons.clear();

        if (_current_file_indices.empty())
        {
            _browser_state.current_file_index = 0;
            _cursor_sprite.set_visible(false);

            if (!_pop_up)
            {
                _pop_up.emplace("Empty folder!", true, false, &_cursor_sprite);
                _pop_up->render();
            }

            return true;
        }

        _cursor_sprite.set_visible(true);

        if (_browser_state.current_file_index < 0)
            _browser_state.current_file_index = 0;

        if (_browser_state.current_file_index >= _current_file_indices.size())
            _browser_state.current_file_index = _current_file_indices.size() - 1;

        const auto &selected_file = current_file(_browser_state.current_file_index);
        const auto selected_name = selected_file.name();

        int lowerBoundary = (_browser_state.current_file_index / max_file_count_pagination) * max_file_count_pagination;
        int upperBoundary = bn::min(lowerBoundary + max_file_count_pagination, _current_file_indices.size());

        text_helpers::draw_left(_text_generator,
                                selected_file.path.substr(0, selected_file.path.size() - selected_name.size()),
                                screen_left + 20,
                                screen_top + 30,
                                _text_sprites);

        text_helpers::draw_right(_text_generator,
                                 bn::to_string<16>(_browser_state.current_file_index + 1) + "/" + bn::to_string<16>(_current_file_indices.size()),
                                 bn::display::width() / 3 + 20,
                                 screen_top + 30,
                                 _text_sprites);

        for (int index = lowerBoundary; index < upperBoundary; ++index)
        {
            const auto &file = current_file(index);
            const auto file_name = file.name();
            bn::string<max_file_count> text(file_name);

            if (file.is_folder() && file_name != "/" && file_name != ".." && file_name != ".")
                text += "/";

            if (text.size() > max_file_character_length)
                text = text.substr(0, max_file_character_length) + "...";

            int icon_x = file_x - 12;
            int icon_y = file_y + (index - lowerBoundary) * text_spacing_y;

            if (file.is_folder())
                _icons.push_back(bn::sprite_items::folder.create_sprite(icon_x, icon_y));
            else if (file.is_file())
                _icons.push_back(bn::sprite_items::file.create_sprite(icon_x, icon_y));
            else if (file.is_gba_file())
                _icons.push_back(bn::sprite_items::gbacart.create_sprite(icon_x, icon_y));
            else
                _icons.push_back(bn::sprite_items::save.create_sprite(icon_x, icon_y));

            _icons.back().set_bg_priority(1);

            text_helpers::draw_left(_text_generator,
                                    text,
                                    file_x,
                                    icon_y,
                                    _text_sprites);

            bn::string<32> size_text;

            if (file.is_folder())
                size_text = "<DIR>";
            else if (file.size < 1024 * 1024)
                size_text = bn::to_string<32>(file.size / 1024) + "KiB";
            else
                size_text = bn::to_string<32>(file.size / (1024 * 1024)) + "MiB";

            text_helpers::draw_right(_text_generator,
                                     size_text,
                                     bn::display::width() / 3 + 25,
                                     icon_y,
                                     _text_sprites);
        }

        update_cursor_position();

        return false;
    }

    void file_browser::update()
    {
        if (_pop_up)
        {
            _pop_up->update();

            if (!_pop_up->is_open())
            {
                _pop_up.reset();
                _cursor_sprite.set_visible(!_current_file_indices.empty());
            }

            return;
        }

        if (_pending_cart_infos_request)
        {
            auto &cart_api = api::cart_api::instance();
            cart_api.update();

            if (!cart_api.response_available())
                return;

            const auto &cart_infos = cart_api.get_cart_infos_response();
            flash_context::instance().set_current_cart_infos(cart_infos);

            _pending_cart_infos_request = false;
            scene_state_machine::instance().request_scene_state(_requested_scene_type);

            return;
        }

        if (bn::keypad::down_pressed())
        {
            if (!_current_file_indices.empty() && _browser_state.current_file_index < _current_file_indices.size() - 1)
                _browser_state.current_file_index++;

            _browser_state.need_update = true;
            render_file_list();
        }

        if (bn::keypad::up_pressed())
        {
            if (_browser_state.current_file_index > 0)
                _browser_state.current_file_index--;

            _browser_state.need_update = true;
            render_file_list();
        }

        if (!_current_file_indices.empty() && bn::keypad::right_pressed())
        {
            _browser_state.current_file_index += 5;

            if (_browser_state.current_file_index >= _current_file_indices.size())
                _browser_state.current_file_index = _current_file_indices.size() - 1;

            _browser_state.need_update = true;
            render_file_list();
        }

        if (!_current_file_indices.empty() && bn::keypad::left_pressed())
        {
            _browser_state.current_file_index -= 5;

            if (_browser_state.current_file_index < 0)
                _browser_state.current_file_index = 0;

            _browser_state.need_update = true;
            render_file_list();
        }

        if (bn::keypad::a_pressed())
        {
            if (_current_file_indices.empty())
                return;

            const auto &file = current_file(_browser_state.current_file_index);

            if (file.is_folder())
            {
                navigation_entry history_entry = {
                    .folder_id = _browser_state.current_folder_id,
                    .selected_index = _browser_state.current_file_index};

                _history.emplace_back(history_entry);
                _browser_state.current_folder_id = file.id;
                _browser_state.current_file_index = 0;
                _browser_state.need_update = true;

                update_current_files();

                if (render_file_list())
                {
                    if (_history.empty())
                        return;

                    navigation_entry previous = _history.back();
                    _history.pop_back();

                    _browser_state.current_folder_id = previous.folder_id;
                    _browser_state.current_file_index = previous.selected_index;
                    _browser_state.need_update = true;

                    update_current_files();
                }

                return;
            }

            if (file.is_gba_file())
            {
                auto rom_infos = api::rom_info_api::instance().get_current_rom_infos(file);

                if (!rom_infos.has_value())
                    return;

                flash_context::instance().set_current_rom_infos(rom_infos.value());
                _requested_scene_type = scene_type::FLASH_SCREEN;
                _pending_cart_infos_request = true;
                api::cart_api::instance().request_cart_infos();

                return;
            }

            if (file.is_save_file())
            {
                auto save_infos = api::save_info_api::instance().get_current_save_infos(file);

                if (!save_infos.has_value())
                    return;

                flash_context::instance().set_current_save_infos(save_infos.value());
                _requested_scene_type = scene_type::SAVE_PROCESS_SCREEN;
                _pending_cart_infos_request = true;
                api::cart_api::instance().request_cart_infos();

                return;
            }
        }

        if (bn::keypad::b_pressed())
        {
            if (_history.empty())
            {
                _restore_history = false;
                scene_state_machine::instance().request_scene_state(scene_type::MAIN_MENU);
                return;
            }

            navigation_entry previous = _history.back();
            _history.pop_back();

            _browser_state.current_folder_id = previous.folder_id;
            _browser_state.current_file_index = previous.selected_index;
            _browser_state.need_update = true;

            update_current_files();
            render_file_list();

            return;
        }
    }

    void file_browser::update_cursor_position()
    {
        if (_pop_up || _current_file_indices.empty())
            return;

        int lowerBoundary = (_browser_state.current_file_index / max_file_count_pagination) * max_file_count_pagination;
        int visible_index = _browser_state.current_file_index - lowerBoundary;

        _cursor_sprite.set_y(file_y + visible_index * text_spacing_y);
    }

    void file_browser::update_current_files()
    {
        const auto &files = api::filesystem_api::instance().get_files_response();
        _current_file_indices.clear();

        for (int index = 0; index < files.size(); ++index)
        {
            if (files[index].parentId == _browser_state.current_folder_id)
                _current_file_indices.push_back(static_cast<uint8_t>(index));
        }
    }

    file_browser_snapshot file_browser::get_snapshot() const
    {
        file_browser_snapshot snapshot;

        snapshot.state = _browser_state;
        snapshot.history = _history;
        snapshot.file_filter = _file_filter;

        return snapshot;
    }

    bool file_browser::restore_snapshot(const file_browser_snapshot &snapshot)
    {
        bool same_filter = snapshot.file_filter.has_value() == _file_filter.has_value();

        if (same_filter && snapshot.file_filter.has_value())
            same_filter = snapshot.file_filter.value() == _file_filter.value();

        if (!same_filter)
            return false;

        _browser_state = snapshot.state;
        _history = snapshot.history;

        update_current_files();

        if (_current_file_indices.empty())
            _browser_state.current_file_index = 0;
        else
            _browser_state.current_file_index = bn::min(_browser_state.current_file_index, _current_file_indices.size() - 1);

        _browser_state.need_update = true;
        render_file_list();

        return true;
    }

    bool file_browser::restore_browser_state()
    {
        return _restore_history;
    }
}
