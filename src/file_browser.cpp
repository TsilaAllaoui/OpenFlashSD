#include "pop_up.h"
#include "bn_string.h"
#include "bn_keypad.h"
#include "bn_algorithm.h"
#include "bn_sprite_palette_ptr.h"
#include "bn_sprite_items_cursor.h"

#include "api/cart_api.h"
#include "file_browser.h"
#include "flash_context.h"
#include "utilities/async.h"
#include "api/rom_info_api.h"
#include "api/filesystem_api.h"
#include "scene_state_machine.h"
#include "bn_sprite_items_file.h"
#include "bn_sprite_items_folder.h"
#include "utilities/text_helpers.h"
#include "bn_sprite_items_gbacart.h"
#include "common_variable_8x16_sprite_font.h"

constexpr int max_file_character_length = 22;

namespace openflash
{
    file_browser::file_browser()
        : _files(),
          _text_generator(
              common::variable_8x16_sprite_font),
          _text_sprites(bn::vector<bn::sprite_ptr, max_file_count>()),
          _cursor_sprite(bn::sprite_items::cursor.create_sprite(
              screen_left + 12,
              file_y)),
          _current_depth_files(bn::vector<file_entry, max_file_count>()),
          _icons(bn::vector<bn::sprite_ptr, max_file_count_pagination>()),
          _browser_state(),
          _restore_history(true)
    {
        _browser_state = {
            .current_folder_id = -1,
            .current_file_index = 0,
            .need_update = true};

        _cursor_sprite.set_visible(false);
    }

    bool file_browser::load_files()
    {
        _files.clear();

        // getting files from server side
        _files = api::filesystem_api::instance().get_files();

        // updating current depth files
        for (const auto &current_file : _files)
        {
            if (current_file.depth == 0 && current_file.parentId == -1)
                _current_depth_files.emplace_back(current_file);
        }

        _cursor_sprite.set_visible(true);

        render_file_list();

        return true;
    }

    bool file_browser::render_file_list()
    {
        if (!_browser_state.need_update)
            return false;

        _browser_state.need_update = false;

        // if no file is on the list (empty directory)
        if (_current_depth_files.empty())
        {
            // Pop up
            auto popup = pop_up("Empty folder!", true, &_cursor_sprite);
            popup.render();
            popup.update();
            return true;
        }

        auto current_file = _current_depth_files[_browser_state.current_file_index];

        int lowerBoundary = 0;
        int upperBoundary = max_file_count_pagination;

        if (_browser_state.current_file_index >= max_file_count_pagination)
        {
            lowerBoundary = _browser_state.current_file_index % max_file_count_pagination + 1;
            upperBoundary = lowerBoundary + max_file_count_pagination;
            if (upperBoundary > _current_depth_files.size())
            {
                upperBoundary = _current_depth_files.size() - 1;
            }
        }

        _text_sprites.clear();
        _icons.clear();

        text_helpers::draw_left(_text_generator,
                                current_file.path.substr(0, current_file.path.size() - current_file.name.size()),
                                screen_left + 20,
                                screen_top + 30,
                                _text_sprites);

        text_helpers::draw_right(_text_generator,
                                 bn::to_string<16>(_browser_state.current_file_index + 1) + "/" + bn::to_string<16>(_current_depth_files.size()),
                                 bn::display::width() / 3 + 20,
                                 screen_top + 30,
                                 _text_sprites);

        for (int index = lowerBoundary; index < bn::min(upperBoundary, _current_depth_files.size()); ++index)
        {
            auto file = _current_depth_files[index];

            bn::string<max_file_count> text(file.name);

            if (file.type == openflash::file_type::FOLDER && file.name != "/" && file.name != ".." && file.name != ".")
            {
                text += "/";
            }

            if (text.size() > max_file_character_length)
            {
                text = text.substr(0, max_file_character_length) + "...";
            }

            if (file.is_folder())
                _icons.push_back(bn::sprite_items::folder.create_sprite(file_x - 12, file_y + (index - lowerBoundary) * text_spacing_y));
            else if (file.is_file())
                _icons.push_back(bn::sprite_items::file.create_sprite(file_x - 12, file_y + (index - lowerBoundary) * text_spacing_y));
            else
                _icons.push_back(bn::sprite_items::gbacart.create_sprite(file_x - 12, file_y + (index - lowerBoundary) * text_spacing_y));

            text_helpers::draw_left(_text_generator,
                                    text,
                                    file_x,
                                    file_y + (index - lowerBoundary) * text_spacing_y,
                                    _text_sprites);

            text_helpers::draw_right(_text_generator,
                                     file.is_folder() ? "<DIR>" : (file.size < 1000 ? (bn::to_string<32>(file.size) + "KiB") : (bn::to_string<32>(file.size % 10) + "MiB")),
                                     bn::display::width() / 3 + 25,
                                     file_y + (index - lowerBoundary) * text_spacing_y,
                                     _text_sprites);
        }

        return false;
    }

    void file_browser::update()
    {
        if (bn::keypad::down_pressed())
        {
            if (_browser_state.current_file_index < _current_depth_files.size() - 1)
            {
                _browser_state.current_file_index++;
            }
            if (_browser_state.current_file_index <= bn::min(max_file_count_pagination - 1, _current_depth_files.size() - 1))
            {
                _cursor_sprite.set_y(file_y + _browser_state.current_file_index * text_spacing_y);
            }
            _browser_state.need_update = true;
            render_file_list();
        }
        if (bn::keypad::up_pressed())
        {
            if (_browser_state.current_file_index > 0)
            {
                _browser_state.current_file_index--;
                if (_browser_state.current_file_index / max_file_count_pagination < 1)
                {
                    _cursor_sprite.set_y(file_y + _browser_state.current_file_index * text_spacing_y);
                }
            }
            _browser_state.need_update = true;
            render_file_list();
        }
        if (bn::keypad::a_pressed())
        {
            if (_current_depth_files.empty())
            {
                return;
            }

            auto file = _current_depth_files[_browser_state.current_file_index];

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

                _cursor_sprite.set_y(file_y);

                // if folder is empty, go back
                if (render_file_list())
                {
                    if (_history.empty())
                    {
                        return;
                    }

                    navigation_entry previous = _history.back();
                    _history.pop_back();

                    _browser_state.current_folder_id = previous.folder_id;
                    _browser_state.current_file_index = previous.selected_index;
                    _browser_state.need_update = true;

                    update_current_files();

                    int cursor_index = bn::min(_browser_state.current_file_index, max_file_count_pagination - 1);

                    _cursor_sprite.set_y(file_y + cursor_index * text_spacing_y);

                    render_file_list();
                }

                return;
            }

            if (file.is_gba_file())
            {
                // update rom infos
                auto rom_infos = api::rom_info_api::instance().get_current_rom_infos(file);
                if (rom_infos.has_value())
                    flash_context::instance().set_current_rom_infos(rom_infos.value());

                // update cart infos
                auto cart_infos = api::cart_api::instance().get_current_cart_infos();
                if (cart_infos.has_value())
                    flash_context::instance().set_current_cart_infos(cart_infos.value());

                scene_state_machine::instance().request_scene_state(scene_type::FLASH_SCREEN);
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

            int cursor_index = bn::min(_browser_state.current_file_index, max_file_count_pagination - 1);

            _cursor_sprite.set_y(file_y + cursor_index * text_spacing_y);

            render_file_list();

            return;
        }
    }

    file_entry *file_browser::find_file_entry_by_id(int id)
    {
        auto result = bn::find_if(_files.begin(), _files.end(), [id](const file_entry &file)
                                  { return file.id == id; });

        if (result != _files.end())
        {
            return result;
        }

        return nullptr;
    }

    void file_browser::update_current_files()
    {
        _current_depth_files.clear();

        for (const file_entry &file : _files)
        {
            if (file.parentId == _browser_state.current_folder_id)
            {
                _current_depth_files.push_back(file);
            }
        }
    }

    file_browser_snapshot file_browser::get_snapshot() const
    {
        file_browser_snapshot snapshot;

        snapshot.state = _browser_state;
        snapshot.history = _history;

        return snapshot;
    }

    void file_browser::restore_snapshot(const file_browser_snapshot &snapshot)
    {
        _browser_state = snapshot.state;
        _history = snapshot.history;

        update_current_files();

        _browser_state.need_update = true;

        int cursor_index =
            _browser_state.current_file_index %
            max_file_count_pagination;

        _cursor_sprite.set_y(
            file_y +
            cursor_index * text_spacing_y);

        render_file_list();
    }

    bool file_browser::restore_browser_state()
    {
        return _restore_history;
    }
}