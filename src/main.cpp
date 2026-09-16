#include "bn_core.h"
#include "scene_state_machine.h"

int main()
{
    bn::core::init();

    // Set first scene
    openflash::scene_state_machine::instance().set_current_scene_state(openflash::scene_type::FILE_BROWSER);
    
    while (true)
    {
        openflash::scene_state_machine::instance().render_current_scene();
        bn::core::update();
    }
}