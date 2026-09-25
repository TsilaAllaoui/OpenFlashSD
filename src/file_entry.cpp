#include "file_entry.h"

namespace openflash
{
    file_entry::file_entry(bn::string_view path_,
                           file_type type_,
                           int16_t id_,
                           int16_t parentId_,
                           uint8_t depth_,
                           uint32_t size_)
        : path(path_),
          size(size_),
          id(id_),
          parentId(parentId_),
          type(type_),
          depth(depth_)
    {
    }

    bn::string_view file_entry::name() const
    {
        if (path.empty() || path == "/")
            return path;

        int index = path.size() - 1;

        while (index >= 0 && path[index] != '/')
            --index;

        return path.substr(index + 1);
    }

    bool file_entry::is_folder() const
    {
        return type == file_type::FOLDER;
    }

    bool file_entry::is_file() const
    {
        return type == file_type::NORMAL_FILE;
    }

    bool file_entry::is_gba_file() const
    {
        return type == file_type::GBA_FILE;
    }

    bool file_entry::is_save_file() const
    {
        return type == file_type::SAVE_FILE;
    }
}
