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

    scene_state_machine::~scene_state_machine()
    {
        if (_current_scene)
            delete _current_scene;
        _current_scene = nullptr;
    }

    scene_state_machine &scene_state_machine::instance()
    {
        static scene_state_machine instance;
        return instance;
    }

    IScene *scene_state_machine::get_current_scene_state()
    {
        return _current_scene;
    }

    void scene_state_machine::set_current_scene_state(scene_type type)
    {
        if (type == scene_type::FILE_BROWSER)
            _current_scene = &_file_brower_scene;
        // Add more scenes here, not exception handling for now
    }

    void scene_state_machine::render_current_scene()
    {
        _current_scene->render();
    }
}