#ifndef FLASH_CONTEXT_H
#define FLASH_CONTEXT_H

#include "bn_optional.h"
#include "file_entry.h"
#include "cart_infos.h"
#include "rom_infos.h"


namespace openflash
{
    class flash_context
    {
    private:
        flash_context();

        bn::optional<rom_infos> _current_rom_infos;
        bn::optional<cart_infos> _current_cart_infos;

    public:
        ~flash_context() = default;
        static flash_context &instance();
        void set_current_rom_infos(const rom_infos &infos);
        bn::optional<rom_infos> get_current_rom_infos();
        void set_current_cart_infos(const cart_infos &infos);
        bn::optional<cart_infos> get_current_cart_infos();
    };
}

#endif // FLASH_CONTEXT_H