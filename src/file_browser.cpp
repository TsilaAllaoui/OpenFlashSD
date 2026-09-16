#include "bn_string.h"
#include "bn_keypad.h"
#include "bn_sprite_palette_ptr.h"
#include "bn_sprite_items_sdcard.h"
#include "bn_sprite_items_cursor.h"

#include "file_browser.h"
#include "common_variable_8x16_sprite_font.h"
#include "bn_sprite_items_folder.h"
#include "bn_sprite_items_file.h"
#include "bn_sprite_items_gbacart.h"

namespace openflash
{
    file_browser::file_browser()
        : _files(),
          _text_generator(
              common::variable_8x16_sprite_font),
          _text_sprites(bn::vector<bn::sprite_ptr, max_file_count>()),
          _current_selected_file(0),
          _current_cursor_position(0),
          _sdcard_sprite(bn::sprite_items::sdcard.create_sprite(
              screen_left + 12,
              screen_top + 12)),
          _cursor_sprite(bn::sprite_items::cursor.create_sprite(
              screen_left + 12,
              file_y)),
          _current_depth(0),
          _previous_parentId(-1),
          _current_depth_files(bn::vector<file_entry, max_file_count>()),
          _icons(bn::vector<bn::sprite_ptr, max_file_count_pagination>())
    {
        bn::sprite_palette_ptr shared_palette =
            _sdcard_sprite.palette();

        _text_generator.set_left_alignment();

        // header
        _text_generator.generate(
            screen_left + 80,
            screen_top + 12,
            "OpenFlashSD",
            _text_sprites);
    }

    bool file_browser::load_files()
    {
        _files.push_back(file_entry("GAMES", "/GAMES", file_type::FOLDER, 0, -1, 0));

        // GAMES/Pokemon
        _files.push_back(file_entry("Pokemon", "/GAMES/Pokemon", file_type::FOLDER, 1, 0, 1));
        _files.push_back(file_entry("Pokemon Emerald.gba", "/GAMES/Pokemon/Pokemon Emerald.gba", file_type::GBA_FILE, 2, 1, 2));
        _files.push_back(file_entry("Pokemon FireRed.gba", "/GAMES/Pokemon/Pokemon FireRed.gba", file_type::GBA_FILE, 3, 1, 2));

        _files.push_back(file_entry("Hacks", "/GAMES/Pokemon/Hacks", file_type::FOLDER, 4, 1, 2));
        _files.push_back(file_entry("Pokemon Unbound.gba", "/GAMES/Pokemon/Hacks/Pokemon Unbound.gba", file_type::GBA_FILE, 5, 4, 3));
        _files.push_back(file_entry("Pokemon Radical Red.gba", "/GAMES/Pokemon/Hacks/Pokemon Radical Red.gba", file_type::GBA_FILE, 6, 4, 3));

        _files.push_back(file_entry("Tools", "/GAMES/Pokemon/Hacks/Tools", file_type::FOLDER, 7, 4, 3));
        _files.push_back(file_entry("Patcher.gba", "/GAMES/Pokemon/Hacks/Tools/Patcher.gba", file_type::GBA_FILE, 8, 7, 4));
        _files.push_back(file_entry("Editor.gba", "/GAMES/Pokemon/Hacks/Tools/Editor.gba", file_type::GBA_FILE, 9, 7, 4));

        // GAMES/Mario
        _files.push_back(file_entry("Mario", "/GAMES/Mario", file_type::FOLDER, 10, 0, 1));
        _files.push_back(file_entry("Super Mario Advance.gba", "/GAMES/Mario/Super Mario Advance.gba", file_type::GBA_FILE, 11, 10, 2));
        _files.push_back(file_entry("Mario Kart Super Circuit.gba", "/GAMES/Mario/Mario Kart Super Circuit.gba", file_type::GBA_FILE, 12, 10, 2));
        _files.push_back(file_entry("Mario Party Advance.gba", "/GAMES/Mario/Mario Party Advance.gba", file_type::GBA_FILE, 13, 10, 2));

        _files.push_back(file_entry("Versions", "/GAMES/Mario/Versions", file_type::FOLDER, 14, 10, 2));

        _files.push_back(file_entry("USA", "/GAMES/Mario/Versions/USA", file_type::FOLDER, 15, 14, 3));
        _files.push_back(file_entry("Mario USA.gba", "/GAMES/Mario/Versions/USA/Mario USA.gba", file_type::GBA_FILE, 16, 15, 4));

        _files.push_back(file_entry("Europe", "/GAMES/Mario/Versions/Europe", file_type::FOLDER, 17, 14, 3));
        _files.push_back(file_entry("Mario Europe.gba", "/GAMES/Mario/Versions/Europe/Mario Europe.gba", file_type::GBA_FILE, 18, 17, 4));

        _files.push_back(file_entry("Japan", "/GAMES/Mario/Versions/Japan", file_type::FOLDER, 19, 14, 3));
        _files.push_back(file_entry("Mario Japan.gba", "/GAMES/Mario/Versions/Japan/Mario Japan.gba", file_type::GBA_FILE, 20, 19, 4));

        // GAMES/Zelda
        _files.push_back(file_entry("Zelda", "/GAMES/Zelda", file_type::FOLDER, 21, 0, 1));
        _files.push_back(file_entry("The Minish Cap.gba", "/GAMES/Zelda/The Minish Cap.gba", file_type::GBA_FILE, 22, 21, 2));
        _files.push_back(file_entry("A Link to the Past.gba", "/GAMES/Zelda/A Link to the Past.gba", file_type::GBA_FILE, 23, 21, 2));

        _files.push_back(file_entry("Randomizers", "/GAMES/Zelda/Randomizers", file_type::FOLDER, 24, 21, 2));
        _files.push_back(file_entry("Minish Cap Randomizer.gba", "/GAMES/Zelda/Randomizers/Minish Cap Randomizer.gba", file_type::GBA_FILE, 25, 24, 3));

        // GAMES/Wario
        _files.push_back(file_entry("Wario", "/GAMES/Wario", file_type::FOLDER, 26, 0, 1));
        _files.push_back(file_entry("Wario Land 4.gba", "/GAMES/Wario/Wario Land 4.gba", file_type::GBA_FILE, 27, 26, 2));
        _files.push_back(file_entry("WarioWare Twisted.gba", "/GAMES/Wario/WarioWare Twisted.gba", file_type::GBA_FILE, 28, 26, 2));

        // Empty folder
        _files.push_back(file_entry("EmptyFolder", "/GAMES/EmptyFolder", file_type::FOLDER, 29, 0, 1));

        // ============================================================
        // SAVES
        // ============================================================

        _files.push_back(file_entry("SAVES", "/SAVES", file_type::FOLDER, 30, -1, 0));

        _files.push_back(file_entry("Pokemon", "/SAVES/Pokemon", file_type::FOLDER, 31, 30, 1));
        _files.push_back(file_entry("Pokemon Emerald.sav", "/SAVES/Pokemon/Pokemon Emerald.sav", file_type::NORMAL_FILE, 32, 31, 2));
        _files.push_back(file_entry("Pokemon FireRed.sav", "/SAVES/Pokemon/Pokemon FireRed.sav", file_type::NORMAL_FILE, 33, 31, 2));

        _files.push_back(file_entry("Backups", "/SAVES/Backups", file_type::FOLDER, 34, 30, 1));
        _files.push_back(file_entry("2026", "/SAVES/Backups/2026", file_type::FOLDER, 35, 34, 2));
        _files.push_back(file_entry("January", "/SAVES/Backups/2026/January", file_type::FOLDER, 36, 35, 3));
        _files.push_back(file_entry("backup.sav", "/SAVES/Backups/2026/January/backup.sav", file_type::NORMAL_FILE, 37, 36, 4));

        // ============================================================
        // ROMS
        // ============================================================

        _files.push_back(file_entry("ROMS", "/ROMS", file_type::FOLDER, 38, -1, 0));

        _files.push_back(file_entry("GBA", "/ROMS/GBA", file_type::FOLDER, 39, 38, 1));
        _files.push_back(file_entry("Metroid Fusion.gba", "/ROMS/GBA/Metroid Fusion.gba", file_type::GBA_FILE, 40, 39, 2));
        _files.push_back(file_entry("Metroid Zero Mission.gba", "/ROMS/GBA/Metroid Zero Mission.gba", file_type::GBA_FILE, 41, 39, 2));
        _files.push_back(file_entry("Castlevania - Aria of Sorrow.gba", "/ROMS/GBA/Castlevania - Aria of Sorrow.gba", file_type::GBA_FILE, 42, 39, 2));

        _files.push_back(file_entry("GBC", "/ROMS/GBC", file_type::FOLDER, 43, 38, 1));
        _files.push_back(file_entry("Pokemon Crystal.gbc", "/ROMS/GBC/Pokemon Crystal.gbc", file_type::NORMAL_FILE, 44, 43, 2));
        _files.push_back(file_entry("Zelda Oracle of Ages.gbc", "/ROMS/GBC/Zelda Oracle of Ages.gbc", file_type::NORMAL_FILE, 45, 43, 2));

        // ============================================================
        // APPS
        // ============================================================

        _files.push_back(file_entry("APPS", "/APPS", file_type::FOLDER, 46, -1, 0));

        _files.push_back(file_entry("Flash", "/APPS/Flash", file_type::FOLDER, 47, 46, 1));
        _files.push_back(file_entry("FlashTool.gba", "/APPS/Flash/FlashTool.gba", file_type::GBA_FILE, 48, 47, 2));
        _files.push_back(file_entry("Flash1M Patcher.gba", "/APPS/Flash/Flash1M Patcher.gba", file_type::GBA_FILE, 49, 47, 2));

        _files.push_back(file_entry("Utilities", "/APPS/Utilities", file_type::FOLDER, 50, 46, 1));
        _files.push_back(file_entry("Save Manager.gba", "/APPS/Utilities/Save Manager.gba", file_type::GBA_FILE, 51, 50, 2));

        // ============================================================
        // CONFIG
        // ============================================================

        _files.push_back(file_entry("CONFIG", "/CONFIG", file_type::FOLDER, 52, -1, 0));

        _files.push_back(file_entry("Themes", "/CONFIG/Themes", file_type::FOLDER, 53, 52, 1));
        _files.push_back(file_entry("Default", "/CONFIG/Themes/Default", file_type::FOLDER, 54, 53, 2));
        _files.push_back(file_entry("theme.cfg", "/CONFIG/Themes/Default/theme.cfg", file_type::NORMAL_FILE, 55, 54, 3));

        _files.push_back(file_entry("settings.ini", "/CONFIG/settings.ini", file_type::NORMAL_FILE, 56, 52, 1));

        // ============================================================
        // TOOLS
        // ============================================================

        _files.push_back(file_entry("TOOLS", "/TOOLS", file_type::FOLDER, 57, -1, 0));

        _files.push_back(file_entry("Development", "/TOOLS/Development", file_type::FOLDER, 58, 57, 1));

        _files.push_back(file_entry("GBA", "/TOOLS/Development/GBA", file_type::FOLDER, 59, 58, 2));
        _files.push_back(file_entry("ROM Checker.gba", "/TOOLS/Development/GBA/ROM Checker.gba", file_type::GBA_FILE, 60, 59, 3));
        _files.push_back(file_entry("Header Editor.gba", "/TOOLS/Development/GBA/Header Editor.gba", file_type::GBA_FILE, 61, 59, 3));

        _files.push_back(file_entry("Debug", "/TOOLS/Development/Debug", file_type::FOLDER, 62, 58, 2));
        _files.push_back(file_entry("debug.log", "/TOOLS/Development/Debug/debug.log", file_type::NORMAL_FILE, 63, 62, 3));

        // ============================================================
        // ROOT FILES
        // ============================================================

        _files.push_back(file_entry("README.txt", "/README.txt", file_type::NORMAL_FILE, 64, -1, 0));
        _files.push_back(file_entry("boot.gba", "/boot.gba", file_type::GBA_FILE, 65, -1, 0));
        _files.push_back(file_entry("LICENSE.txt", "/LICENSE.txt", file_type::NORMAL_FILE, 66, -1, 0));
        _files.push_back(file_entry("VERSION.txt", "/VERSION.txt", file_type::NORMAL_FILE, 67, -1, 0));

        update_current_files();

        render_file_list();

        return true;
    }

    bn::vector<file_entry, max_file_count> file_browser::get_files()
    {
        return _files;
    }

    void file_browser::render_file_list()
    {
        int lowerBoundary = 0;
        int upperBoundary = max_file_count_pagination;

        if (_current_selected_file >= max_file_count_pagination)
        {
            lowerBoundary = _current_selected_file % max_file_count_pagination + 1;
            upperBoundary = lowerBoundary + max_file_count_pagination;
            if (upperBoundary > _current_depth_files.size())
            {
                upperBoundary = _current_depth_files.size() - 1;
            }
        }

        _text_sprites.clear();
        _icons.clear();
        for (int index = lowerBoundary; index < bn::min(upperBoundary, _current_depth_files.size()); ++index)
        {
            auto file = _current_depth_files[index];

            bn::string<max_file_count> text(file.name);

            if (file.type == openflash::file_type::FOLDER && file.name != "/" && file.name != ".." && file.name != ".")
            {
                text += "/";
            }

            if (file.is_folder())
                _icons.push_back(bn::sprite_items::folder.create_sprite(file_x - 12, file_y + (index - lowerBoundary) * text_spacing_y));
            else if (file.is_file())
                _icons.push_back(bn::sprite_items::file.create_sprite(file_x - 12, file_y + (index - lowerBoundary) * text_spacing_y));
            else
                _icons.push_back(bn::sprite_items::gbacart.create_sprite(file_x - 12, file_y + (index - lowerBoundary) * text_spacing_y));

            _text_generator.generate(
                bn::fixed(file_x),
                bn::fixed(file_y + (index - lowerBoundary) * text_spacing_y),
                text.c_str(),
                _text_sprites);
        }
    }

    void file_browser::update()
    {
        if (bn::keypad::down_pressed())
        {
            if (_current_selected_file < _current_depth_files.size() - 1)
            {
                ++_current_selected_file;
            }
            if (_current_cursor_position < bn::min(max_file_count_pagination - 1, _current_depth_files.size() - 1))
            {
                _cursor_sprite.set_y(file_y + ++_current_cursor_position * text_spacing_y);
            }
            render_file_list();
        }
        else if (bn::keypad::up_pressed())
        {
            if (_current_selected_file > 0)
            {
                if (_current_cursor_position > 0 && _current_selected_file / max_file_count_pagination < 1)
                {
                    _current_cursor_position--;
                }
                _cursor_sprite.set_y(file_y + _current_cursor_position * text_spacing_y);
                _current_selected_file--;
            }
            render_file_list();
        }
        else if (bn::keypad::a_pressed())
        {
            auto file = _current_depth_files[_current_selected_file];
            if (file.is_folder())
            {
                _current_depth = file.depth + 1;
                _previous_parentId = file.id;
                _cursor_sprite.set_y(file_y);
                update_current_files();
                render_file_list();
            }
        }
        else if (bn::keypad::b_pressed())
        {
            if (_current_depth == 0)
                return;

            auto file = _current_depth_files[_current_selected_file];
            _cursor_sprite.set_y(file_y);
            if (_current_depth > 0)
                _current_depth--;
            _previous_parentId = _files[file.parentId].parentId;

            update_current_files();
            render_file_list();
        }
    }

    void file_browser::update_current_files()
    {
        _current_selected_file = 0,
        _current_cursor_position = 0,
        _current_depth_files.clear();
        for (const auto &file : _files)
        {
            if (file.depth == _current_depth && file.parentId == _previous_parentId)
            {
                _current_depth_files.emplace_back(file);
            }
        }
    }
}