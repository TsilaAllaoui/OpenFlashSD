#ifndef ISCENE_H
#define ISCENE_H

#include "scenes/scene_type.h"

namespace openflash
{
    class i_scene
    {
      public:
        virtual ~i_scene() = default;
        virtual void enter() = 0;
        virtual void exit() = 0;
        virtual void render() = 0;
        virtual void update() = 0;
        virtual scene_type get_scene_type() = 0;
        virtual void set_title(const bn::string_view &title) = 0;
    };
} // namespace openflash

#endif // ISCENE_H