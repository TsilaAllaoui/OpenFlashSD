#include "process_info_api.h"
#include "bn_random.h"

bn::random random;

namespace openflash
{
    namespace api
    {
        process_info_api::process_info_api()
            : _current_process_infos()
        {
        }

        process_info_api &process_info_api::instance()
        {
            static process_info_api api;
            return api;
        }

        bn::optional<process_infos> process_info_api::get_current_process_infos()
        {
            static int frames = 0;
            // get process infos from server (esp32)
            if (!_current_process_infos.has_value())
            {
                _current_process_infos.emplace();
                _current_process_infos->type = process_type::FLASHING;
                _current_process_infos->elapsed_time.minutes = 1;
                _current_process_infos->elapsed_time.seconds = 24;
                _current_process_infos->estimated_time.minutes = 5;
                _current_process_infos->estimated_time.seconds = 12;
            }
            if (frames >= 2)
            {
                frames = 0;
                if (_current_process_infos->progress < 100)
                    _current_process_infos->progress++;
                else
                    _current_process_infos->progress = 0;
                _current_process_infos->speed = random.get_int(500);
            }
            frames++;
            _current_process_infos->status = (_current_process_infos->type == process_type::DUMPING ? process_status::READING : process_status::WRITING);
            return _current_process_infos;
        }

        void process_info_api::reset()
        {
            _current_process_infos.reset();
        }
    }
}
