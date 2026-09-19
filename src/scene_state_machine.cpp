#include "bn_bg_tiles.h"
#include "bn_regular_bg_ptr.h"
#include "bn_regular_bg_item.h"
#include "bn_regular_bg_map_ptr.h"
#include "bn_regular_bg_tiles_items_tiles.h"

#include "tile_maps.h"
#include "scene_state_machine.h"

namespace openflash
{
    scene_state_machine::scene_state_machine()
    {
        _current_scene = nullptr;
    }

    scene_state_machine &scene_state_machine::instance()
    {
        static scene_state_machine instance;
        return instance;
    }

    i_scene *scene_state_machine::get_current_scene_state()
    {
        return _current_scene;
    }

    void scene_state_machine::set_current_scene_state(scene_type type)
    {
        // Free the last scene
        if (_current_scene)
            _current_scene->exit();

        if (type == scene_type::MAIN_MENU)
            _current_scene = &_main_menu_scene;
        else if (type == scene_type::FILE_BROWSER)
            _current_scene = &_file_brower_scene;
        else if (type == scene_type::FLASH_SCREEN)
            _current_scene = &_flash_screen_scene;
        // Add more scenes here, not exception handling for now

        _current_scene->enter();
    }

    void scene_state_machine::render_current_scene()
    {
        _current_scene->render();
    }

    void scene_state_machine::update_current_scene()
    {
        if (_current_scene)
            _current_scene->update();

        if (_requested_scene)
        {
            scene_type next_scene = *_requested_scene;
            _requested_scene.reset();

            set_current_scene_state(next_scene);
        }
    }

    void scene_state_machine::request_scene_state(scene_type type)
    {
        _requested_scene = type;
    }
}