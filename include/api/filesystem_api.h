#ifndef FILESYSTEM_API_H
#define FILESYSTEM_API_H

#include "bn_vector.h"

#include "file_entry.h"

namespace openflash
{
    namespace api
    {
        class filesystem_api
        {
        private:
            filesystem_api() = default;
            ~filesystem_api() = default;

            bn::vector<file_entry, max_file_count> _files;

        public:
        static filesystem_api& instance();
            bn::vector<file_entry, max_file_count> get_files();
        };
    }
}

#endif // FILESYSTEM_H