#ifndef SAVE_INFO_API_H
#define SAVE_INFO_API_H

#include "save_infos.h"

namespace openflash
{
    namespace api
    {
        class save_info_api
        {
        private:
            save_info_api() = default;

            bn::optional<save_infos> _current_save_infos;

        public:
            ~save_info_api() = default;
            static save_info_api &instance();
            bn::optional<save_infos> get_current_save_infos(const file_entry& file);
        };
    }
}

#endif // SAVE_INFO_API_H