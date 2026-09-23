#ifndef SAVE_PROCESS_SELECTION_SCENE_H
#define SAVE_PROCESS_SELECTION_SCENE_H

#include "bn_display.h"
#include "bn_sprite_ptr.h"
#include "bn_regular_bg_ptr.h"
#include "bn_sprite_text_generator.h"

#include "cart_infos.h"
#include "utilities/pop_up.h"
#include "scenes/i_scene.h"
#include "scenes/scene_type.h"

constexpr int save_process_selection_scene_max_text_sprite_count = 100;
constexpr int save_process_selection_scene_text_y_spacing = 14;
constexpr int save_process_selection_scene_text_y_top = -bn::display::height() / 2 + 30;
constexpr int save_process_selection_x_alignment = -bn::display::width() / 2 + 20;

namespace openflash
{
    class save_process_selection_scene : public i_scene
    {
    private:
        bn::string_view _title;
        scene_type _type;
        bn::optional<bn::regular_bg_ptr> _background;
        bn::optional<bn::regular_bg_ptr> _big_selector_bg;
        bn::optional<pop_up> _pop_up;
        bn::sprite_text_generator _text_generator_8x16;
        bn::sprite_text_generator _text_generator_8x8;
        bn::vector<bn::sprite_ptr, save_process_selection_scene_max_text_sprite_count> _text_sprites;
        bn::vector<bn::sprite_ptr, 7> _sprites;
        cart_infos _current_cart_infos;
        int _current_selector_index;

    public:
        save_process_selection_scene();
        virtual ~save_process_selection_scene() = default;
        virtual void enter();
        virtual void exit();
        virtual void update();
        virtual void render();
        virtual scene_type get_scene_type();
        virtual void set_title(const bn::string_view &title);
    };
}

#endif // SAVE_PROCESS_SELECTION_SCENE_H
