#ifndef PROCESS_PROGRESS_H
#define PROCESS_PROGRESS_H

#include "bn_display.h"
#include "bn_sprite_ptr.h"
#include "bn_regular_bg_ptr.h"
#include "bn_sprite_text_generator.h"

#include "cart_infos.h"
#include "scenes/i_scene.h"
#include "scenes/scene_type.h"


constexpr int process_progress_scene_max_text_sprite_count = 100;
constexpr int process_progress_scene_text_y_spacing = 14;
constexpr int process_progress_scene_text_y_top = -bn::display::height() / 2 + 30;
constexpr int process_progress_x_alignment = -bn::display::width() / 2 + 20;

namespace openflash
{
    struct time
    {
        int minutes;
        int seconds;
    };

    enum class process_status
    {
        WRITING,
        READING,
        ERASING,
        IDLE
    };

    struct process_infos
    {
        int progress;
        int speed;
        time elapsed_time;
        time estimated_time;
        process_status status;
    };

    class process_progress_scene : public i_scene
    {
    private:
        bn::string_view _title;
        scene_type _type;
        bn::optional<bn::regular_bg_ptr> _background;
        bn::optional<bn::regular_bg_ptr> _pop_up_bg;
        bn::sprite_text_generator _text_generator;
        bn::vector<bn::sprite_ptr, process_progress_scene_max_text_sprite_count> _text_sprites;

        int gbacart_index = 0;
        int arrow_index = 1;
        int save_1_index = 2;
        int save_2_index = 3;
        bn::vector<bn::sprite_ptr, 4> _sprites;

        bn::optional<cart_infos> _current_cart_infos;
        process_infos _current_process_infos;

    public:
        process_progress_scene();
        virtual ~process_progress_scene() = default;
        virtual void enter();
        virtual void exit();
        virtual void update();
        virtual void render();
        virtual scene_type get_scene_type();
        virtual void set_title(const bn::string_view& title);
        void update_screen_infos();
    };
}

#endif // PROCESS_PROGRESS_H