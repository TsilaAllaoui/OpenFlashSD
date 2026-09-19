#include "pop_up.h"
#include "bn_string.h"
#include "bn_keypad.h"
#include "bn_algorithm.h"
#include "bn_sprite_palette_ptr.h"
#include "bn_sprite_items_cursor.h"

#include "api/cart_api.h"
#include "file_browser.h"
#include "flash_context.h"
#include "scene_state_machine.h"
#include "api/rom_info_api.h"
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
          _browser_state()
    {
        _browser_state = {
            .current_folder_id = -1,
            .current_file_index = 0,
            .need_update = true};
    }

    bool file_browser::load_files()
    {
        _files.push_back(file_entry("GAMES", "/GAMES", file_type::FOLDER, 0, -1, 0, 0));

        // GAMES/Pokemon
        _files.push_back(file_entry("Pokemon", "/GAMES/Pokemon", file_type::FOLDER, 1, 0, 1, 0));
        _files.push_back(file_entry("Pokemon Emerald.gba", "/GAMES/Pokemon/Pokemon Emerald.gba", file_type::GBA_FILE, 2, 1, 2, 16777216));
        _files.push_back(file_entry("Pokemon FireRed.gba", "/GAMES/Pokemon/Pokemon FireRed.gba", file_type::GBA_FILE, 3, 1, 2, 16777216));

        _files.push_back(file_entry("Hacks", "/GAMES/Pokemon/Hacks", file_type::FOLDER, 4, 1, 2, 0));
        _files.push_back(file_entry("Pokemon Unbound.gba", "/GAMES/Pokemon/Hacks/Pokemon Unbound.gba", file_type::GBA_FILE, 5, 4, 3, 33554432));
        _files.push_back(file_entry("Pokemon Radical Red.gba", "/GAMES/Pokemon/Hacks/Pokemon Radical Red.gba", file_type::GBA_FILE, 6, 4, 3, 33554432));

        _files.push_back(file_entry("Tools", "/GAMES/Pokemon/Hacks/Tools", file_type::FOLDER, 7, 4, 3, 0));
        _files.push_back(file_entry("Patcher.gba", "/GAMES/Pokemon/Hacks/Tools/Patcher.gba", file_type::GBA_FILE, 8, 7, 4, 524288));
        _files.push_back(file_entry("Editor.gba", "/GAMES/Pokemon/Hacks/Tools/Editor.gba", file_type::GBA_FILE, 9, 7, 4, 1048576));

        // GAMES/Mario
        _files.push_back(file_entry("Mario", "/GAMES/Mario", file_type::FOLDER, 10, 0, 1, 0));
        _files.push_back(file_entry("Super Mario Advance.gba", "/GAMES/Mario/Super Mario Advance.gba", file_type::GBA_FILE, 11, 10, 2, 4194304));
        _files.push_back(file_entry("Mario Kart Super Circuit.gba", "/GAMES/Mario/Mario Kart Super Circuit.gba", file_type::GBA_FILE, 12, 10, 2, 4194304));
        _files.push_back(file_entry("Mario Party Advance.gba", "/GAMES/Mario/Mario Party Advance.gba", file_type::GBA_FILE, 13, 10, 2, 8388608));

        _files.push_back(file_entry("Versions", "/GAMES/Mario/Versions", file_type::FOLDER, 14, 10, 2, 0));

        _files.push_back(file_entry("USA", "/GAMES/Mario/Versions/USA", file_type::FOLDER, 15, 14, 3, 0));
        _files.push_back(file_entry("Mario USA.gba", "/GAMES/Mario/Versions/USA/Mario USA.gba", file_type::GBA_FILE, 16, 15, 4, 4194304));

        _files.push_back(file_entry("Europe", "/GAMES/Mario/Versions/Europe", file_type::FOLDER, 17, 14, 3, 0));
        _files.push_back(file_entry("Mario Europe.gba", "/GAMES/Mario/Versions/Europe/Mario Europe.gba", file_type::GBA_FILE, 18, 17, 4, 4194304));

        _files.push_back(file_entry("Japan", "/GAMES/Mario/Versions/Japan", file_type::FOLDER, 19, 14, 3, 0));
        _files.push_back(file_entry("Mario Japan.gba", "/GAMES/Mario/Versions/Japan/Mario Japan.gba", file_type::GBA_FILE, 20, 19, 4, 4194304));

        // GAMES/Zelda
        _files.push_back(file_entry("Zelda", "/GAMES/Zelda", file_type::FOLDER, 21, 0, 1, 0));
        _files.push_back(file_entry("The Minish Cap.gba", "/GAMES/Zelda/The Minish Cap.gba", file_type::GBA_FILE, 22, 21, 2, 16777216));
        _files.push_back(file_entry("A Link to the Past.gba", "/GAMES/Zelda/A Link to the Past.gba", file_type::GBA_FILE, 23, 21, 2, 8388608));

        _files.push_back(file_entry("Randomizers", "/GAMES/Zelda/Randomizers", file_type::FOLDER, 24, 21, 2, 0));
        _files.push_back(file_entry("Minish Cap Randomizer.gba", "/GAMES/Zelda/Randomizers/Minish Cap Randomizer.gba", file_type::GBA_FILE, 25, 24, 3, 16777216));

        // GAMES/Wario
        _files.push_back(file_entry("Wario", "/GAMES/Wario", file_type::FOLDER, 26, 0, 1, 0));
        _files.push_back(file_entry("Wario Land 4.gba", "/GAMES/Wario/Wario Land 4.gba", file_type::GBA_FILE, 27, 26, 2, 8388608));
        _files.push_back(file_entry("WarioWare Twisted.gba", "/GAMES/Wario/WarioWare Twisted.gba", file_type::GBA_FILE, 28, 26, 2, 16777216));

        // Empty folder
        _files.push_back(file_entry("EmptyFolder", "/GAMES/EmptyFolder", file_type::FOLDER, 29, 0, 1, 0));

        // ============================================================
        // SAVES
        // ============================================================

        _files.push_back(file_entry("SAVES", "/SAVES", file_type::FOLDER, 30, -1, 0, 0));

        _files.push_back(file_entry("Pokemon", "/SAVES/Pokemon", file_type::FOLDER, 31, 30, 1, 0));
        _files.push_back(file_entry("Pokemon Emerald.sav", "/SAVES/Pokemon/Pokemon Emerald.sav", file_type::NORMAL_FILE, 32, 31, 2, 131072));
        _files.push_back(file_entry("Pokemon FireRed.sav", "/SAVES/Pokemon/Pokemon FireRed.sav", file_type::NORMAL_FILE, 33, 31, 2, 131072));

        _files.push_back(file_entry("Backups", "/SAVES/Backups", file_type::FOLDER, 34, 30, 1, 0));
        _files.push_back(file_entry("2026", "/SAVES/Backups/2026", file_type::FOLDER, 35, 34, 2, 0));
        _files.push_back(file_entry("January", "/SAVES/Backups/2026/January", file_type::FOLDER, 36, 35, 3, 0));
        _files.push_back(file_entry("backup.sav", "/SAVES/Backups/2026/January/backup.sav", file_type::NORMAL_FILE, 37, 36, 4, 131072));

        // ============================================================
        // ROMS
        // ============================================================

        _files.push_back(file_entry("ROMS", "/ROMS", file_type::FOLDER, 38, -1, 0, 0));

        _files.push_back(file_entry("GBA", "/ROMS/GBA", file_type::FOLDER, 39, 38, 1, 0));
        _files.push_back(file_entry("Metroid Fusion.gba", "/ROMS/GBA/Metroid Fusion.gba", file_type::GBA_FILE, 40, 39, 2, 8388608));
        _files.push_back(file_entry("Metroid Zero Mission.gba", "/ROMS/GBA/Metroid Zero Mission.gba", file_type::GBA_FILE, 41, 39, 2, 8388608));
        _files.push_back(file_entry("Castlevania - Aria of Sorrow.gba", "/ROMS/GBA/Castlevania - Aria of Sorrow.gba", file_type::GBA_FILE, 42, 39, 2, 8388608));

        _files.push_back(file_entry("GBC", "/ROMS/GBC", file_type::FOLDER, 43, 38, 1, 0));
        _files.push_back(file_entry("Pokemon Crystal.gbc", "/ROMS/GBC/Pokemon Crystal.gbc", file_type::NORMAL_FILE, 44, 43, 2, 2097152));
        _files.push_back(file_entry("Zelda Oracle of Ages.gbc", "/ROMS/GBC/Zelda Oracle of Ages.gbc", file_type::NORMAL_FILE, 45, 43, 2, 1048576));

        // ============================================================
        // APPS
        // ============================================================

        _files.push_back(file_entry("APPS", "/APPS", file_type::FOLDER, 46, -1, 0, 0));

        _files.push_back(file_entry("Flash", "/APPS/Flash", file_type::FOLDER, 47, 46, 1, 0));
        _files.push_back(file_entry("FlashTool.gba", "/APPS/Flash/FlashTool.gba", file_type::GBA_FILE, 48, 47, 2, 524288));
        _files.push_back(file_entry("Flash1M Patcher.gba", "/APPS/Flash/Flash1M Patcher.gba", file_type::GBA_FILE, 49, 47, 2, 262144));

        _files.push_back(file_entry("Utilities", "/APPS/Utilities", file_type::FOLDER, 50, 46, 1, 0));
        _files.push_back(file_entry("Save Manager.gba", "/APPS/Utilities/Save Manager.gba", file_type::GBA_FILE, 51, 50, 2, 524288));

        // ============================================================
        // CONFIG
        // ============================================================

        _files.push_back(file_entry("CONFIG", "/CONFIG", file_type::FOLDER, 52, -1, 0, 0));

        _files.push_back(file_entry("Themes", "/CONFIG/Themes", file_type::FOLDER, 53, 52, 1, 0));
        _files.push_back(file_entry("Default", "/CONFIG/Themes/Default", file_type::FOLDER, 54, 53, 2, 0));
        _files.push_back(file_entry("theme.cfg", "/CONFIG/Themes/Default/theme.cfg", file_type::NORMAL_FILE, 55, 54, 3, 2048));

        _files.push_back(file_entry("settings.ini", "/CONFIG/settings.ini", file_type::NORMAL_FILE, 56, 52, 1, 1024));

        // ============================================================
        // TOOLS
        // ============================================================

        _files.push_back(file_entry("TOOLS", "/TOOLS", file_type::FOLDER, 57, -1, 0, 0));

        _files.push_back(file_entry("Development", "/TOOLS/Development", file_type::FOLDER, 58, 57, 1, 0));

        _files.push_back(file_entry("GBA", "/TOOLS/Development/GBA", file_type::FOLDER, 59, 58, 2, 0));
        _files.push_back(file_entry("ROM Checker.gba", "/TOOLS/Development/GBA/ROM Checker.gba", file_type::GBA_FILE, 60, 59, 3, 262144));
        _files.push_back(file_entry("Header Editor.gba", "/TOOLS/Development/GBA/Header Editor.gba", file_type::GBA_FILE, 61, 59, 3, 262144));

        _files.push_back(file_entry("Debug", "/TOOLS/Development/Debug", file_type::FOLDER, 62, 58, 2, 0));
        _files.push_back(file_entry("debug.log", "/TOOLS/Development/Debug/debug.log", file_type::NORMAL_FILE, 63, 62, 3, 8192));

        // ============================================================
        // ROOT FILES
        // ============================================================

        _files.push_back(file_entry("README.txt", "/README.txt", file_type::NORMAL_FILE, 64, -1, 0, 4096));
        _files.push_back(file_entry("boot.gba", "/boot.gba", file_type::GBA_FILE, 65, -1, 0, 262144));
        _files.push_back(file_entry("LICENSE.txt", "/LICENSE.txt", file_type::NORMAL_FILE, 66, -1, 0, 2048));
        _files.push_back(file_entry("VERSION.txt", "/VERSION.txt", file_type::NORMAL_FILE, 67, -1, 0, 32));

        // updating current depth files
        for (const auto &current_file : _files)
        {
            if (current_file.depth == 0 && current_file.parentId == -1)
                _current_depth_files.emplace_back(current_file);
        }

        render_file_list();

        return true;
    }

    bn::vector<file_entry, max_file_count> file_browser::get_files()
    {
        return _files;
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
            auto popup = pop_up("Empty folder!", &_cursor_sprite);
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
}