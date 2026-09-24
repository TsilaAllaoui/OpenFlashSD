#include "bn_assert.h"
#include "bn_bg_tiles.h"
#include "bn_regular_bg_ptr.h"
#include "bn_regular_bg_item.h"
#include "bn_regular_bg_map_ptr.h"
#include "bn_regular_bg_tiles_items_tiles.h"

#include "main_menu_bg.h"
#include "scene_state_machine.h"

namespace openflash
{
    scene_state_machine::scene_state_machine() : _current_scene(nullptr)
    {
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
        if (_current_scene)
            _current_scene->exit();

        _current_scene = nullptr;

        if (type == scene_type::MAIN_MENU)
            _current_scene = &_main_menu_bg_scene;
        else if (type == scene_type::FILE_BROWSER)
            _current_scene = &_file_brower_scene;
        else if (type == scene_type::FLASH_SCREEN)
            _current_scene = &_flash_screen_bg_scene;
        else if (type == scene_type::DUMP_ROM_INFO)
            _current_scene = &_dump_rom_info_scene;
        else if (type == scene_type::PROCESS_PROGRESS)
            _current_scene = &_process_progress_scene;
        else if (type == scene_type::SAVE_PROCESS_SCREEN)
            _current_scene = &_process_save_info_scene;
        else if (type == scene_type::SAVE_PROCESS_SELECTION_SCREEN)
            _current_scene = &_save_process_selection_scene;

        BN_ASSERT(_current_scene, "Invalid scene type");
        _requested_scene.reset();
        _current_scene->enter();
    }

    void scene_state_machine::render_current_scene()
    {
        if (_current_scene)
            _current_scene->render();
    }

    void scene_state_machine::update_current_scene()
    {
        if (_current_scene)
            _current_scene->update();

        if (_requested_scene)
        {
            scene_type next_scene = *_requested_scene;
            set_current_scene_state(next_scene);
        }
    }

    void scene_state_machine::request_scene_state(scene_type type)
    {
        BN_ASSERT(type != scene_type::PROCESS_PROGRESS, "Use request_process_progress() for PROCESS_PROGRESS");
        _requested_scene = type;
    }

    void scene_state_machine::request_process_progress(process_type type)
    {
        _process_progress_scene.set_process_type(type);
        _requested_scene = scene_type::PROCESS_PROGRESS;
    }

    scene_type scene_state_machine::get_last_request_scene()
    {
        BN_ASSERT(_requested_scene.has_value(), "No requested scene");
        return _requested_scene.value();
    }
}
