#ifndef DUMP_ROM_INFO_SCENE_H
#define DUMP_ROM_INFO_SCENE_H

#include "bn_display.h"
#include "bn_sprite_ptr.h"
#include "bn_regular_bg_ptr.h"
#include "bn_sprite_text_generator.h"

#include "scenes/scene_type.h"
#include "scenes/i_scene.h"

constexpr int dump_scene_max_text_sprite_count = 100;
constexpr int dump_scene_text_y_spacing = 16;
constexpr int dump_scene_text_y_top = -bn::display::height() / 2 + 35;

namespace openflash
{
    class dump_rom_info_scene : public i_scene
    {
    private:
        scene_type _type;
        bn::optional<bn::regular_bg_ptr> _background;
        bn::optional<bn::regular_bg_ptr> _pop_up_bg;
        bn::sprite_text_generator _text_generator;
        bn::vector<bn::sprite_ptr, dump_scene_max_text_sprite_count> _text_sprites;
        bn::sprite_ptr _gbacart_sprite;

    public:
        dump_rom_info_scene();
        virtual ~dump_rom_info_scene() = default;
        virtual void enter();
        virtual void exit();
        virtual void update();
        virtual void render();
        virtual scene_type get_scene_type();
    };
}

#endif // DUMP_ROM_INFO_SCENE_H