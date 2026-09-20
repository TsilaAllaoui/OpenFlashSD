#ifndef DUMP_ROM_INFO_SCENE_H
#define DUMP_ROM_INFO_SCENE_H

#include "bn_display.h"
#include "bn_sprite_ptr.h"
#include "bn_regular_bg_ptr.h"
#include "bn_sprite_text_generator.h"

#include "cart_infos.h"
#include "scenes/i_scene.h"
#include "scenes/scene_type.h"


constexpr int dump_scene_max_text_sprite_count = 100;
constexpr int dump_scene_text_y_spacing = 14;
constexpr int dump_scene_text_y_top = -bn::display::height() / 2 + 30;
constexpr int dump_x_alignment = -bn::display::width() / 2 + 20;

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

        int gbacart_index = 0;
        int arrow_index = 1;
        int save_1_index = 2;
        int save_2_index = 3;
        bn::vector<bn::sprite_ptr, 4> _sprites;

        cart_infos _current_cart_infos;

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