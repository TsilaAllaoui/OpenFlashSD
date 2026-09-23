#include "process_info_api.h"
#include "bn_random.h"

namespace openflash
{
    namespace api
    {
        namespace
        {
            bn::random process_random;
        }
        process_info_api::process_info_api()
            : _current_process_infos(),
              _frames(0)
        {
        }

        process_info_api &process_info_api::instance()
        {
            static process_info_api api;
            return api;
        }

        void process_info_api::start(process_type type)
        {
            _current_process_infos.emplace();
            _frames = 0;
            _current_process_infos->progress = 0;
            _current_process_infos->speed = 0;
            _current_process_infos->elapsed_time = {0, 0};
            _current_process_infos->estimated_time = {0, 0};
            _current_process_infos->type = type;

            if (type == process_type::DUMPING || type == process_type::BACKUP_SAVE)
                _current_process_infos->status = process_status::READING;
            else
                _current_process_infos->status = process_status::WRITING;
        }

        bn::optional<process_infos> process_info_api::get_current_process_infos()
        {
            if (!_current_process_infos.has_value())
                return bn::nullopt;

            if (_frames >= 2)
            {
                _frames = 0;

                if (_current_process_infos->progress < 100)
                    _current_process_infos->progress++;

                _current_process_infos->speed = process_random.get_int(500);
            }

            _frames++;
            return _current_process_infos;
        }

        void process_info_api::reset()
        {
            _current_process_infos.reset();
            _frames = 0;
        }
    }
}
