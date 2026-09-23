#ifndef PROCESS_INFO_API_H
#define PROCESS_INFO_API_H

#include "bn_optional.h"
#include "process_infos.h"

namespace openflash
{
    namespace api
    {
        class process_info_api
        {
        private:
            process_info_api();

            bn::optional<process_infos> _current_process_infos;
            int _frames;

        public:
            ~process_info_api() = default;
            static process_info_api &instance();
            void start(process_type type);
            bn::optional<process_infos> get_current_process_infos();
            void reset();
        };
    }
}

#endif // PROCESS_INFO_API_H
