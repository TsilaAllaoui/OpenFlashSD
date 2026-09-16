#ifndef FILE_BROWSER_SCENE_H
#define FILE_BROWSER_SCENE_H

#include "IScene.h"
#include "file_browser.h"
#include "bn_regular_bg_ptr.h"

namespace openflash
{
    class file_brower_scene : public IScene
    {
    private:
        file_browser _file_browser;
        scene_type _type;
        bn::regular_bg_ptr _background;
        bn::sprite_text_generator _text_generator;
        bn::vector<bn::sprite_ptr, 32> _text_sprites;

    public:
        file_brower_scene();
        virtual ~file_brower_scene() = default;
        virtual void render();
        virtual scene_type get_scene_type();
        void render_background();
    };
}

#endif // FILE_BROWSER_SCENE_H