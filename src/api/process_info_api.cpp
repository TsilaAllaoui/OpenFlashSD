#include "process_info_api.h"
#include "bn_assert.h"

#ifdef USEMOCK
#include "bn_random.h"
#endif

namespace openflash
{
    namespace api
    {
#ifdef USEMOCK
        namespace
        {
            bn::random process_random;
        }
#endif

        process_info_api::process_info_api()
            : _current_process_infos(),
              _loading(false),
              _response_ready(false)
#ifdef USEMOCK
              , _mock_frames(0)
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
            _current_process_infos->type = type;
            _current_process_infos->status = type == process_type::DUMPING || type == process_type::BACKUP_SAVE ? process_status::READING : process_status::WRITING;
            _loading = false;
            _response_ready = false;
#ifdef USEMOCK
            _mock_frames = 0;
#else
            // Start process on ESP32 here.
#endif
        }

        void process_info_api::request_process_infos()
        {
            if (!_current_process_infos.has_value())
                BN_ERROR("Process is not started");

            _loading = true;
            _response_ready = false;
#ifdef USEMOCK
            _mock_frames = 15;
#else
            // Request process information from ESP32 here.
#endif
        }

        void process_info_api::update()
        {
            if (!_loading)
                return;

#ifdef USEMOCK
            if (_mock_frames > 0)
            {
                _mock_frames--;
                return;
            }

            if (!_current_process_infos.has_value())
                BN_ERROR("Process infos is nullopt");

            if (_current_process_infos->progress < 100)
                _current_process_infos->progress++;

            _current_process_infos->speed = process_random.get_int(500);
            _loading = false;
            _response_ready = true;
#else
            // Poll ESP32 response here. Keep _loading true until a complete response is available.
#endif
        }

        bool process_info_api::response_available() const
        {
            return _response_ready && _current_process_infos.has_value();
        }

        const process_infos &process_info_api::get_process_infos_response() const
        {
            if (!_current_process_infos.has_value())
                BN_ERROR("Process infos is nullopt");

            return *_current_process_infos;
        }

        void process_info_api::request_process_infos_reset()
        {
#ifdef USEMOCK
            _mock_frames = 0;
#else
            // Reset process state on ESP32 here.
#endif
            _loading = false;
            _response_ready = false;
            _current_process_infos.reset();
        }
    }
}
