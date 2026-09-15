#ifndef FILE_ENTRY_H
#define FILE_ENTRY_H

#include "bn_string_view.h"
#include "bn_display.h"

constexpr int screen_left = -(bn::display::width() / 2);
constexpr int screen_top = -(bn::display::height() / 2);
constexpr int text_spacing_y = 10;
constexpr int file_x = screen_left + 20;
constexpr int file_y = screen_top + 35;
constexpr int max_file_count_pagination = 12;

namespace openflash
{
    enum class file_type
    {
        NORMAL_FILE,
        FOLDER
    };

    struct file_entry
    {
        bn::string_view name;
        bn::string_view path;
        file_type type;

        file_entry(bn::string_view name_,
                   bn::string_view path_,
                   file_type type_ = file_type::NORMAL_FILE)
            : name(name_), path(path_), type(type_) {}
    };
}

#endif // FILE_ENTRY_H