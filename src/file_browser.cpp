#include "bn_string.h"
#include "bn_keypad.h"
#include "bn_sprite_palette_ptr.h"
#include "bn_sprite_items_sdcard.h"
#include "bn_sprite_items_cursor.h"

#include "file_browser.h"
#include "common_variable_8x16_sprite_font.h"

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
              screen_left + 14,
              file_y))
    {
        bn::sprite_palette_ptr shared_palette =
            _sdcard_sprite.palette();

        _cursor_sprite.set_palette(shared_palette);

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
        _files.push_back(file_entry("CONFIG", "/CONFIG", file_type::FOLDER));
        _files.push_back(file_entry("GB", "/GB", file_type::FOLDER));
        _files.push_back(file_entry("GBA", "/GBA", file_type::FOLDER));
        _files.push_back(file_entry("Rom1.gba", "/Rom1.gba"));
        _files.push_back(file_entry("Rom2.gba", "/Rom2.gba"));
        _files.push_back(file_entry("Rom3.gba", "/Rom3.gba"));
        _files.push_back(file_entry("Rom4.gba", "/Rom4.gba"));
        _files.push_back(file_entry("config.cfg", "/config.cfg"));
        _files.push_back(file_entry("test", "/test", file_type::FOLDER));
        _files.push_back(file_entry("GBC", "/GBC", file_type::FOLDER));
        _files.push_back(file_entry("Backup", "/Backup", file_type::FOLDER));
        _files.push_back(file_entry("saves", "/saves", file_type::FOLDER));
        _files.push_back(file_entry("log.txt", "/log.txt"));
        _files.push_back(file_entry("CFG.cfg", "/CFG.cfg"));
        _files.push_back(file_entry("hotkey", "/hotkey", file_type::FOLDER));
        _files.push_back(file_entry("New_Folder", "/New_Folder", file_type::FOLDER));
        _files.push_back(file_entry("New", "/New", file_type::FOLDER));
        _files.push_back(file_entry("Test.gba", "/Test.gba"));
        _files.push_back(file_entry("sram.gba", "/sram.gba"));

        render_file_list();

        return true;
    }

    bn::vector<file_entry, max_file_count> file_browser::get_files()
    {
        return _files;
    }

    void file_browser::render()
    {
        if (bn::keypad::down_pressed())
        {
            if (_current_selected_file < _files.size() - 1)
            {
                ++_current_selected_file;
            }
            if (_current_cursor_position < max_file_count_pagination - 1)
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
    }

    void file_browser::render_file_list()
    {
        int lowerBoundary = 0;
        int upperBoundary = max_file_count_pagination;

        if (_current_selected_file >= max_file_count_pagination)
        {
            lowerBoundary = _current_selected_file % max_file_count_pagination + 1;
            upperBoundary = lowerBoundary + max_file_count_pagination;
            if (upperBoundary > _files.size())
            {
                upperBoundary = _files.size() - 1;
            }
        }

        _text_sprites.clear();
        for (int index = lowerBoundary; index < upperBoundary; ++index)
        {
            bn::string<max_file_count> text(_files[index].name);

            if (_files[index].type == openflash::file_type::FOLDER)
            {
                text += "/";
            }

            _text_generator.generate(
                bn::fixed(file_x),
                bn::fixed(file_y + (index - lowerBoundary) * text_spacing_y),
                text.c_str(),
                _text_sprites);
        }
    }
}