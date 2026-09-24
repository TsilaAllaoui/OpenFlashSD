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
            bool _loading;
            bool _response_ready;
#ifdef USEMOCK
            int _mock_frames;
#endif

          public:
            ~process_info_api() = default;
            static process_info_api &instance();
            void start_process(process_type type);
            void request_process_infos();
            void update();
            bool response_available() const;
            const process_infos &get_process_infos_response() const;
            void request_process_infos_reset();
        };
    } // namespace api
} // namespace openflash

#endif // PROCESS_INFO_API_H
