#include "flash_context.h"

namespace openflash
{
    flash_context::flash_context()
    {
    }

    flash_context &flash_context::instance()
    {
        static flash_context context;
        return context;
    }

    void flash_context::set_current_rom_infos(const rom_infos &infos)
    {
        _current_rom_infos = infos;
    }

    bn::optional<rom_infos> flash_context::get_current_rom_infos()
    {
        return _current_rom_infos;
    }

    void flash_context::set_current_cart_infos(const cart_infos &infos)
    {
        _current_cart_infos = infos;
    }

    bn::optional<cart_infos> flash_context::get_current_cart_infos()
    {
        return _current_cart_infos;
    }

    void flash_context::set_current_file_filter(const bn::optional<file_type> &type)
    {
        _current_file_filter = type;
    }

    bn::optional<file_type> flash_context::get_current_file_filter()
    {
        return _current_file_filter;
    }

    void flash_context::set_current_save_infos(const save_infos &infos)
    {
        _current_save_infos = infos;
    }

    bn::optional<save_infos> flash_context::get_current_save_infos()
    {
        return _current_save_infos;
    }
} // namespace openflash
