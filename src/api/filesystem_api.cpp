#include "bn_core.h"
#include "filesystem_api.h"
#include "utilities/async.h"
#include "utilities/pop_up.h"
#include "mock/mocks.h"

namespace openflash
{
    namespace api
    {
        filesystem_api &api::filesystem_api::instance()
        {
            static filesystem_api api;
            return api;
        }

        bn::vector<file_entry, max_file_count> api::filesystem_api::get_files()
        {
            auto popup = pop_up("Loading files...", false);

            // simulating wait time
            async::delay(60);

            // get files from server side (esp32)
            auto files = mock::mock_file_entries();
            _files.clear();
            for (const auto &file : files)
                _files.emplace_back(file);
            return _files;
        }
    }
}
