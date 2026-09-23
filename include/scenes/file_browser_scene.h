#ifndef FILE_BROWSER_SCENE_H
#define FILE_BROWSER_SCENE_H

#include "i_scene.h"
#include "file_browser.h"
#include "bn_regular_bg_ptr.h"
#include "utilities/pop_up.h"

namespace openflash
{
    class file_brower_scene : public i_scene
    {
    private:
        bn::string_view _title;
        bn::optional<file_browser> _file_browser;
        bn::optional<pop_up> _loading_popup;
        scene_type _type;
        bn::optional<bn::regular_bg_ptr> _background;
        bn::sprite_text_generator _text_generator;
        bn::vector<bn::sprite_ptr, 32> _text_sprites;
        bn::sprite_ptr _sdcard_sprite;
        bn::optional<file_browser_snapshot> _previous_file_browser;
        bool _loading_files;

        void set_content_priority(int priority);

    public:
        file_brower_scene();
        virtual ~file_brower_scene() = default;
        virtual void enter();
        virtual void exit();
        virtual void update();
        virtual void render();
        virtual scene_type get_scene_type();
        void delete_file_browser_snapshot();
        virtual void set_title(const bn::string_view &title);
    };
}

#endif // FILE_BROWSER_SCENE_H
