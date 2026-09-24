#include "bn_assert.h"

#ifdef USEMOCK
#include "bn_random.h"
#endif

#include "process_info_api.h"

namespace openflash
{
    namespace api
    {
#ifdef USEMOCK
        namespace
        {
            constexpr int mock_response_delay_frames = 15;
            constexpr int mock_response_interval_frames = mock_response_delay_frames + 1;
            bn::random process_random;
        } // namespace
#endif

        process_info_api::process_info_api()
            : _current_process_infos(), _frames(0), _loading(false), _response_ready(false)
#ifdef USEMOCK
              ,
              _mock_frames(0)
#endif
        {
        }

        process_info_api &process_info_api::instance()
        {
            static process_info_api api;
            return api;
        }

        void process_info_api::start_process(process_type type)
        {
            _current_process_infos.emplace();
            _current_process_infos->progress = 0;
            _current_process_infos->speed = 0;
            _current_process_infos->elapsed_time = {0, 0};
            _current_process_infos->estimated_time = {0, 0};
            _current_process_infos->status = type == process_type::DUMPING || type == process_type::BACKUP_SAVE
                                                 ? process_status::READING
                                                 : process_status::WRITING;
            _current_process_infos->type = type;
            _frames = 0;
            _loading = false;
            _response_ready = false;
#ifdef USEMOCK
            _mock_frames = 0;
#else
            // TODO: send the start-process command to the ESP32 here.
#endif
        }

        void process_info_api::request_process_infos()
        {
            BN_ASSERT(_current_process_infos.has_value(), "Process was not started");

            if (_loading)
                return;

            _loading = true;
            _response_ready = false;
#ifdef USEMOCK
            _mock_frames = mock_response_delay_frames;
#else
            // TODO: send the process-info request to the ESP32 here.
#endif
        }

        void process_info_api::update()
        {
            if (!_loading)
                return;

#ifdef USEMOCK
            if (_mock_frames > 0)
            {
                --_mock_frames;
                return;
            }

            BN_ASSERT(_current_process_infos.has_value(), "Process infos is nullopt");

            auto &infos = *_current_process_infos;

            if (infos.progress < 100)
                ++infos.progress;

            infos.speed = process_random.get_int(500);
            _frames += mock_response_interval_frames;

            const int elapsed_seconds = _frames / 60;
            infos.elapsed_time.minutes = elapsed_seconds / 60;
            infos.elapsed_time.seconds = elapsed_seconds % 60;

            const int remaining_frames = (100 - infos.progress) * mock_response_interval_frames;
            const int estimated_seconds = remaining_frames / 60;
            infos.estimated_time.minutes = estimated_seconds / 60;
            infos.estimated_time.seconds = estimated_seconds % 60;
            _loading = false;
            _response_ready = true;
#else
                // TODO: poll the ESP32 process-info response here.
#endif
        }

        bool process_info_api::response_available() const
        {
            return _response_ready && _current_process_infos.has_value();
        }

        const process_infos &process_info_api::get_process_infos_response() const
        {
            BN_ASSERT(_current_process_infos.has_value(), "Process infos is nullopt");
            return *_current_process_infos;
        }

        void process_info_api::request_process_infos_reset()
        {
            _current_process_infos.reset();
            _frames = 0;
            _loading = false;
            _response_ready = false;
#ifdef USEMOCK
            _mock_frames = 0;
#else
            // TODO: reset the ESP32 process state here when needed.
#endif
        }
    } // namespace api
} // namespace openflash
