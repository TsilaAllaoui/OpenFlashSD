#ifndef FLASH_SCREEN_SCENE_H
#define FLASH_SCREEN_SCENE_H

#include "bn_sprite_ptr.h"
#include "bn_regular_bg_ptr.h"
#include "bn_sprite_text_generator.h"

#include "scenes/scene_type.h"
#include "scenes/i_scene.h"


namespace openflash
{
    class flash_screen_scene : public i_scene
    {
    private:
        scene_type _type;
        bn::optional<bn::regular_bg_ptr> _background;
        bn::optional<bn::regular_bg_ptr> _pop_up_bg;
        bn::sprite_text_generator _text_generator;
        bn::vector<bn::sprite_ptr, 32> _text_sprites;
    public:
        flash_screen_scene();
        virtual ~flash_screen_scene() = default;
        virtual void enter();
        virtual void exit();
        virtual void update();
        virtual void render();
        virtual scene_type get_scene_type();
    };
}

#endif // FLASH_SCREEN_SCENE_H