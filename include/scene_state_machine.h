#ifndef SCENE_STATE_MACHINE_H
#define SCENE_STATE_MACHINE_H

#include "file_browser.h"
#include "scenes/i_scene.h"
#include "scenes/main_menu_scene.h"
#include "scenes/file_browser_scene.h"
#include "scenes/flash_screen_scene.h"
#include "scenes/dump_rom_info_scene.h"

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

        bn::optional<scene_type> _requested_scene;

    public:
        ~scene_state_machine() = default;
        static scene_state_machine &instance();
        i_scene *get_current_scene_state();
        void set_current_scene_state(scene_type state);
        void render_current_scene();
        void update_current_scene();
        void request_scene_state(scene_type type);
    };
}

#endif // SCENE_STATE_MACHINE_H