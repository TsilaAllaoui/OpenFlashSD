#ifndef PROCESS_PROGRESS_H
#define PROCESS_PROGRESS_H

#include "bn_display.h"
#include "bn_optional.h"
#include "bn_vector.h"
#include "bn_sprite_ptr.h"
#include "bn_regular_bg_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_regular_bg_map_ptr.h"

#include "cart_infos.h"
#include "api/requests.h"
#include "process_infos.h"
#include "scenes/i_scene.h"
#include "utilities/pop_up.h"
#include "scenes/scene_type.h"

constexpr int process_progress_scene_max_text_sprite_count = 100;
constexpr int process_progress_scene_text_y_spacing = 14;
constexpr int process_progress_scene_text_y_top = -bn::display::height() / 2 + 30;
constexpr int process_progress_x_alignment = -bn::display::width() / 2 + 20;

namespace openflash
{
    class process_progress_scene : public i_scene
    {
      private:
        bn::string_view _title;
        scene_type _type;
        process_type _process_type;
        bool _process_type_ready;
        bn::optional<bn::regular_bg_ptr> _background;
        bn::optional<bn::regular_bg_map_ptr> _bg_map;
        bn::optional<pop_up> _pop_up;
        bn::sprite_text_generator _text_generator;
        bn::vector<bn::sprite_ptr, process_progress_scene_max_text_sprite_count> _text_sprites;

        int gbacart_index = 0;
        int arrow_index = 1;
        int save_1_index = 2;
        int save_2_index = 3;
        bn::vector<bn::sprite_ptr, 4> _sprites;

        bn::optional<cart_infos> _current_cart_infos;

        int _progress_index;
        int old_progress_index;
        request_status _pending_process_infos_request;
        request_status _pending_cart_infos_request;

        void set_content_priority(int priority);

      public:
        process_progress_scene();
        virtual ~process_progress_scene() = default;
        virtual void enter();
        virtual void exit();
        virtual void update();
        virtual void render();
        virtual scene_type get_scene_type();
        virtual void set_title(const bn::string_view &title);
        void set_process_type(process_type type);
        void set_tile(int x, int y, int tile_index);
        void update_screen_infos();
    };
} // namespace openflash

#endif // PROCESS_PROGRESS_H
