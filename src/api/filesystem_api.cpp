#include "mock/mocks.h"
#include "api/filesystem_api.h"

namespace openflash
{
    namespace api
    {
        filesystem_api::filesystem_api()
            : _files(),
              _file_filter(),
              _loading(false),
              _response_ready(false),
              _mock_frames(0)
        {
        }

        filesystem_api &filesystem_api::instance()
        {
            static filesystem_api api;
            return api;
        }

        void filesystem_api::request_files(bn::optional<file_type> file_filter)
        {
            _file_filter = file_filter;
            _files.clear();
            _loading = true;
            _response_ready = false;
            _mock_frames = 30;
        }

        void filesystem_api::update()
        {
            if (!_loading)
                return;

            if (_mock_frames > 0)
            {
                _mock_frames--;
                return;
            }

            const auto &files = mock::mock_file_entries();

            for (const auto &file : files)
            {
                if (_file_filter.has_value() &&
                    file.type != *_file_filter &&
                    file.type != file_type::FOLDER)
                {
                    continue;
                }

                _files.emplace_back(file);
            }

            _loading = false;
            _response_ready = true;
        }

        bool filesystem_api::response_available() const
        {
            return _response_ready;
        }

        const bn::vector<file_entry, max_file_count> &filesystem_api::get_files_response() const
        {
            return _files;
        }
    }
}
