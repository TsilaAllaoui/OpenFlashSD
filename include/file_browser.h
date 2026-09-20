#ifndef FILE_BROWSER_H
#define FILE_BROWSER_H

#include "bn_string.h"
#include "file_entry.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"

constexpr int max_character_count = 64;
constexpr int max_navigation_depth = 16;

namespace openflash
{
    struct file_browser_state
    {
        int current_folder_id = -1;
        int current_file_index = 0;
        bool need_update = true;
    };

    struct navigation_entry
    {
        int folder_id;
        int selected_index;
    };

    struct file_browser_snapshot
    {
        file_browser_state state;

        bn::vector<
            navigation_entry,
            max_navigation_depth>
            history;
    };

    class file_browser
    {
    private:
        bn::vector<file_entry, max_file_count> _files;
        bn::sprite_text_generator _text_generator;
        bn::vector<bn::sprite_ptr, max_file_count> _text_sprites;
        bn::sprite_ptr _cursor_sprite;
        bn::vector<file_entry, max_file_count> _current_depth_files;
        bn::vector<bn::sprite_ptr, max_file_count_pagination> _icons;
        file_browser_state _browser_state;

        bn::vector<navigation_entry, max_navigation_depth> _history;
        
        bool _restore_history;
    public:
        file_browser();
        ~file_browser() = default;

        // load files to list from a given source (ESP32 GPIO, Web, ...)
        bool load_files();

        // render file list
        bool render_file_list();

        // render file browser
        void update();

        // find specific file using id
        file_entry *find_file_entry_by_id(int id);

        // update current files from given selected file
        void update_current_files();

        // get/restore file browser state
        file_browser_snapshot get_snapshot() const;
        void restore_snapshot(const file_browser_snapshot &snapshot);

        // check if we need to restore file browser state
        bool restore_browser_state();
    };
}

#endif // FILE_MANAGER_H