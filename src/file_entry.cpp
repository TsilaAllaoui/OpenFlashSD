#include "file_entry.h"

namespace openflash
{
    file_entry::file_entry(
        bn::string_view name_,
        bn::string_view path_,
        file_type type_,
        int id_,
        int parentId_,
        int depth_,
        int size_)
        : name(name_),
          path(path_),
          type(type_),
          id(id_),
          parentId(parentId_),
          depth(depth_),
          size(size_)
    {
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
}