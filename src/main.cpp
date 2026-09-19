#include "bn_core.h"
#include "scene_state_machine.h"

int main()
{
    bn::core::init();

    // Set first scene
    openflash::scene_state_machine::instance().request_scene_state(openflash::scene_type::MAIN_MENU);
    
    while (true)
    {
        openflash::scene_state_machine::instance().update_current_scene();
        openflash::scene_state_machine::instance().render_current_scene();
        bn::core::update();
    }
}