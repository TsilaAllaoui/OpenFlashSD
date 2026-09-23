#ifndef FLASH_CONTEXT_H
#define FLASH_CONTEXT_H

#include "rom_infos.h"
#include "file_entry.h"
#include "cart_infos.h"
#include "save_infos.h"
#include "bn_optional.h"

namespace openflash
{
    class flash_context
    {
    private:
        flash_context();

        bn::optional<rom_infos> _current_rom_infos;
        bn::optional<cart_infos> _current_cart_infos;
        bn::optional<file_type> _current_file_filter;
        bn::optional<save_infos> _current_save_infos;

    public:
        ~flash_context() = default;
        static flash_context &instance();
        void set_current_rom_infos(const rom_infos &infos);
        bn::optional<rom_infos> get_current_rom_infos();
        void set_current_cart_infos(const cart_infos &infos);
        bn::optional<cart_infos> get_current_cart_infos();
        void set_current_file_filter(const bn::optional<file_type> &type);
        bn::optional<file_type> get_current_file_filter();
        void set_current_save_infos(const save_infos &infos);
        bn::optional<save_infos> get_current_save_infos();
    };
}

#endif // FLASH_CONTEXT_H