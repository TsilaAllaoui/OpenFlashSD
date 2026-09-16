#ifndef ISCENE_H
#define ISCENE_H

#include "scenes/scene_type.h"

namespace openflash
{
    class IScene
    {
    public:
        virtual ~IScene() = default;
        virtual void render() = 0;
        virtual scene_type get_scene_type() = 0;
    };
}

#endif // ISCENE_H