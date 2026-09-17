#ifndef MAIN_MENU_SCENE_H
#define MAIN_MENU_SCENE_H

#include "bn_sprite_ptr.h"
#include "bn_regular_bg_ptr.h"
#include "bn_sprite_text_generator.h"

#include "scenes/scene_type.h"
#include "scenes/IScene.h"


namespace openflash
{
    class main_menu_scene : public IScene
    {
    private:
        scene_type _type;
        bn::optional<bn::regular_bg_ptr> _background;
        bn::sprite_text_generator _text_generator;
        bn::vector<bn::sprite_ptr, 32> _text_sprites;
    public:
        main_menu_scene();
        virtual ~main_menu_scene() = default;
        virtual void enter();
        virtual void exit();
        virtual void update();
        virtual void render();
        virtual scene_type get_scene_type();
    };
}

#endif // MAIN_MENU_SCENE_H