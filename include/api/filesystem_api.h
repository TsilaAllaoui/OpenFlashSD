#ifndef FILESYSTEM_API_H
#define FILESYSTEM_API_H

#include "bn_optional.h"
#include "bn_vector.h"
#include "file_entry.h"

namespace openflash
{
    namespace api
    {
        class filesystem_api
        {
          private:
            filesystem_api();
            ~filesystem_api() = default;

            bn::vector<file_entry, max_file_count> _files;
            bn::optional<file_type> _file_filter;
            bool _loading;
            bool _response_ready;
#ifdef USEMOCK
            int _mock_frames;
#endif

          public:
            static filesystem_api &instance();
            void request_files(bn::optional<file_type> file_filter);
            void update();
            bool response_available() const;
            const bn::vector<file_entry, max_file_count> &get_files_response() const;
        };
    } // namespace api
} // namespace openflash

#endif // FILESYSTEM_API_H
