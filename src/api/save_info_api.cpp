#include "save_info_api.h"
#include "process_infos.h"

namespace openflash
{
    namespace api
    {
        save_info_api &save_info_api::instance()
        {
            static save_info_api api;
            return api;
        }

        bn::optional<save_infos> save_info_api::get_current_save_infos(const file_entry &file)
        {
#ifdef USEMOCK
            _current_save_infos.emplace(file, save_type::FLASH_128K, process_status::WRITING);
#else
            (void) file;
            _current_save_infos.reset();
            // Request save information from ESP32 here.
#endif
            return _current_save_infos;
        }
    }
}
