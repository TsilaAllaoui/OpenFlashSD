#ifndef FILE_BROWSER_H
#define FILE_BROWSER_H

#include "file_entry.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"

constexpr int max_file_count = 100;

namespace openflash
{
    class file_browser
    {
        bn::vector<file_entry, max_file_count> _files;
        bn::sprite_text_generator _text_generator;
        bn::vector<bn::sprite_ptr, max_file_count> _text_sprites;
        int _current_selected_file;
        int _current_cursor_position;
        bn::sprite_ptr _sdcard_sprite;
        bn::sprite_ptr _cursor_sprite;
    public:
        file_browser();
        ~file_browser() = default;

        // load files to list from a given source (ESP32 GPIO, Web, ...)
        bool load_files();

        // get current file list
        bn::vector<file_entry, max_file_count> get_files();

        // render file list
        void render_file_list();

        // render file browser
        void render();
    };
}

#endif // FILE_MANAGER_H