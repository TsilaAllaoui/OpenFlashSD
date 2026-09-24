#ifndef SCENE_STATE_MACHINE_H
#define SCENE_STATE_MACHINE_H

#include "bn_optional.h"
#include "file_browser.h"
#include "process_infos.h"
#include "scenes/i_scene.h"
#include "scenes/main_menu_scene.h"
#include "scenes/file_browser_scene.h"
#include "scenes/flash_screen_scene.h"
#include "scenes/dump_rom_info_scene.h"
#include "scenes/process_progress_scene.h"
#include "scenes/process_save_info_scene.h"
#include "scenes/save_process_selection_scene.h"

namespace openflash
{
    class scene_state_machine
    {
      private:
        scene_state_machine();
        i_scene *_current_scene;
        file_brower_scene _file_brower_scene;
        main_menu_bg_scene _main_menu_bg_scene;
        flash_screen_scene _flash_screen_bg_scene;
        dump_rom_info_scene _dump_rom_info_scene;
        process_progress_scene _process_progress_scene;
        process_save_info_scene _process_save_info_scene;
        save_process_selection_scene _save_process_selection_scene;
        bn::optional<scene_type> _requested_scene;

      public:
        ~scene_state_machine() = default;
        static scene_state_machine &instance();
        i_scene *get_current_scene_state();
        void set_current_scene_state(scene_type state);
        void render_current_scene();
        void update_current_scene();
        void request_scene_state(scene_type type);
        void request_process_progress(process_type type);
        scene_type get_last_request_scene();
    };
} // namespace openflash

#endif // SCENE_STATE_MACHINE_H
