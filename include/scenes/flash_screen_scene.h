#ifndef FLASH_SCREEN_SCENE_H
#define FLASH_SCREEN_SCENE_H

#include "bn_display.h"
#include "bn_sprite_ptr.h"
#include "bn_regular_bg_ptr.h"
#include "bn_sprite_text_generator.h"

#include "scenes/scene_type.h"
#include "scenes/i_scene.h"

constexpr int flash_scene_max_text_sprite_count = 100;
constexpr int flash_scene_text_y_spacing = 14;
constexpr int flash_scene_text_y_top = -bn::display::height() / 2 + 30;
constexpr int flash_x_alignment = -bn::display::width() / 2 + 20;

namespace openflash
{
    class flash_screen_scene : public i_scene
    {
    private:
        bn::string_view _title;
        scene_type _type;
        bn::optional<bn::regular_bg_ptr> _background;
        bn::optional<bn::regular_bg_ptr> _pop_up_bg;
        bn::sprite_text_generator _text_generator_8x16;
        bn::sprite_text_generator _text_generator_8x8;
        bn::vector<bn::sprite_ptr, flash_scene_max_text_sprite_count> _text_sprites;
        bn::sprite_ptr _gbacart_sprite;
    public:
        flash_screen_scene();
        virtual ~flash_screen_scene() = default;
        virtual void enter();
        virtual void exit();
        virtual void update();
        virtual void render();
        virtual scene_type get_scene_type();
        virtual void set_title(const bn::string_view& title);
    };
}

#endif // FLASH_SCREEN_SCENE_H