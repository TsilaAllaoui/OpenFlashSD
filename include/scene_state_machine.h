#ifndef SCENE_STATE_MACHINE_H
#define SCENE_STATE_MACHINE_H

#include "scenes/IScene.h"
#include "file_browser.h"
#include "scenes/file_browser_scene.h"

namespace openflash
{
    class scene_state_machine
    {
    private:
        scene_state_machine();
        IScene *_current_scene;
        file_brower_scene _file_brower_scene;

    public:
        ~scene_state_machine();
        static scene_state_machine &instance();
        IScene *get_current_scene_state();
        void set_current_scene_state(scene_type state);
        void render_current_scene();
    };
}

#endif // SCENE_STATE_MACHINE_H