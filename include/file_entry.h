#ifndef FILE_ENTRY_H
#define FILE_ENTRY_H

#include <stdint.h>

#include "bn_vector.h"
#include "bn_string.h"
#include "bn_display.h"
#include "bn_optional.h"
#include "bn_string_view.h"

constexpr int screen_left = -(bn::display::width() / 2);
constexpr int screen_top = -(bn::display::height() / 2);
constexpr int file_x = screen_left + 40;
constexpr int text_spacing_y = 14;
constexpr int file_y = screen_top + 44;
constexpr int max_file_count_pagination = 8;
constexpr int max_file_count = 100;

namespace openflash
{
    struct rom_infos;

    enum class file_type : uint8_t
    {
        NORMAL_FILE,
        FOLDER,
        GBA_FILE,
        SAVE_FILE
    };

    class file_entry
    {
    public:
        file_entry(bn::string_view path_,
                   file_type type_,
                   int16_t id_,
                   int16_t parentId_ = -1,
                   uint8_t depth_ = 0,
                   uint32_t size_ = 0);

        ~file_entry() = default;

        bn::string_view path;
        uint32_t size;
        int16_t id;
        int16_t parentId;
        file_type type;
        uint8_t depth;

        bn::string_view name() const;
        bool is_folder() const;
        bool is_file() const;
        bool is_gba_file() const;
        bool is_save_file() const;
        rom_infos get_gba_file_info(uint8_t *rom_bytes);
    };
}

#endif // FILE_ENTRY_H
