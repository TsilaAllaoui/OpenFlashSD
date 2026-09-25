#include "bn_array.h"
#include "bn_display.h"
#include "bn_bg_tiles.h"
#include "bn_bg_palette_ptr.h"
#include "bn_regular_bg_ptr.h"
#include "bn_sprite_items_info.h"
#include "bn_sprite_items_arrow.h"
#include "bn_regular_bg_map_ptr.h"
#include "bn_regular_bg_map_cell_info.h"
#include "bn_regular_bg_tiles_ptr.h"
#include "bn_regular_bg_tiles_items_tiles.h"

#include "flash_context.h"
#include "api/process_info_api.h"
#include "scene_state_machine.h"
#include "process_progress_scene.h"
#include "utilities/text_helpers.h"
#include "process_progress_scene_bg.h"
#include "common_variable_8x16_sprite_font.h"

namespace openflash
{
    process_progress_scene::process_progress_scene()
        : _type(scene_type::PROCESS_PROGRESS), _background(), _bg_map(), _pop_up(),
          _text_generator(bn::sprite_text_generator(common::variable_8x16_sprite_font)), _sprites(),
          _current_cart_infos(), _progress_index(3), old_progress_index(_progress_index),
          _pending_process_infos_request(request_status::IDLE)
    {
        _sprites.emplace_back(bn::sprite_items::arrow.create_sprite(5, -10));
        _sprites.emplace_back(bn::sprite_items::info.create_sprite(-bn::display::width() / 2 + 30, 60));

        for (auto &sprite : _sprites)
            sprite.set_visible(false);

        _text_generator.set_bg_priority(1);
    }

    void process_progress_scene::update_screen_infos()
    {
        auto &process_api = api::process_info_api::instance();

        if (_pending_process_infos_request == request_status::PENDING && !process_api.response_available())
            return;

        _pending_process_infos_request = request_status::IDLE;

        const auto &current_process_infos = process_api.get_process_infos_response();
        auto current_process_type = flash_context::instance().get_requested_process_type();

        if (!current_process_type.has_value())
            BN_ERROR("Process type is nullopt");

        process_type type = current_process_type.value();

        if (current_process_infos.progress >= 100)
        {
            if (!_pop_up)
            {
                bn::string<18> popup_header;

                if (type == process_type::DUMPING)
                    popup_header = "Dump complete!";
                else if (type == process_type::FLASHING)
                    popup_header = "Flash complete!";
                else if (type == process_type::BACKUP_SAVE)
                    popup_header = "Backup complete!";
                else
                    popup_header = "Restore complete!";

                set_content_priority(1);
                _pop_up.emplace(popup_header, true, false);
                _pop_up->render();
            }

            return;
        }

        if (old_progress_index != current_process_infos.progress && current_process_infos.progress % 5 == 0
            && current_process_infos.progress > 0)
        {
            old_progress_index = current_process_infos.progress;
            set_tile(_progress_index++, 13, 63);
        }

        _text_sprites.clear();

        text_helpers::draw_centered(_text_generator, _title, screen_top + 12, _text_sprites);

        bn::string<max_character_count> header_text;

        if (type == process_type::DUMPING)
        {
            if (!_current_cart_infos.has_value())
                BN_ERROR("Cart infos is nullopt");

            header_text = "Path: \"/dump/";
            header_text += _current_cart_infos->cart_rom_infos.name;
            header_text += ".gba\"";
        }
        else if (type == process_type::FLASHING)
        {
            auto current_rom_infos = flash_context::instance().get_current_rom_infos();

            if (current_rom_infos.has_value())
            {
                header_text = "Path: \"";
                header_text += current_rom_infos->file_path;
                header_text += "\"";
            }
        }
        else if (type == process_type::BACKUP_SAVE)
        {
            if (!_current_cart_infos.has_value())
                BN_ERROR("Cart infos is nullopt");

            header_text = "Path: \"/saves/";
            header_text += _current_cart_infos->cart_rom_infos.name;
            header_text += ".sav\"";
        }
        else if (type == process_type::RESTORE_SAVE)
        {
            auto current_save_infos = flash_context::instance().get_current_save_infos();

            if (current_save_infos.has_value())
            {
                header_text = "Path: \"";
                header_text += current_save_infos->file.path;
                header_text += "\"";
            }
        }

        if (!header_text.empty())
        {
            text_helpers::draw_centered_at(_text_generator,
                                           text_helpers::truncate_text(header_text, 32),
                                           0,
                                           -48,
                                           _text_sprites);
        }

        text_helpers::draw_centered_at(_text_generator, "DO NOT POWER OFF CONSOLE", 10, 60, _text_sprites);

        text_helpers::draw_centered_at(_text_generator,
                                       bn::to_string<32>(current_process_infos.progress) + "%",
                                       85,
                                       27,
                                       _text_sprites);

        bn::string<32> status_text = "Status: ";

        if (type == process_type::DUMPING)
            status_text += "READING CART";
        else if (type == process_type::FLASHING)
            status_text += "WRITING CART";
        else if (type == process_type::BACKUP_SAVE)
            status_text += "READING SAVE";
        else
            status_text += "WRITING SAVE";

        text_helpers::draw_centered_at(_text_generator, status_text, 0, 15, _text_sprites);

        bn::string<32> elapsed_text = "Elapsed: ";
        elapsed_text += bn::to_string<32>(current_process_infos.elapsed_time.minutes);
        elapsed_text += ":";

        if (current_process_infos.elapsed_time.seconds < 10)
            elapsed_text += "0";

        elapsed_text += bn::to_string<32>(current_process_infos.elapsed_time.seconds);

        text_helpers::draw_centered_at(_text_generator,
                                       elapsed_text,
                                       -bn::display::width() / 2 + 50,
                                       40,
                                       _text_sprites);

        bn::string<32> speed_text = "Speed: ";
        speed_text += bn::to_string<32>(current_process_infos.speed);
        speed_text += "KiB/s";

        text_helpers::draw_centered_at(_text_generator, speed_text, 60, 40, _text_sprites);

        for (auto &sprite : _sprites)
        {
            sprite.set_visible(true);
            sprite.set_bg_priority(1);
        }

        for (auto &sprite : _text_sprites)
            sprite.set_bg_priority(1);

        process_api.request_process_infos();
        _pending_process_infos_request = request_status::PENDING;
    }

    void process_progress_scene::set_tile(int x, int y, int tile_index)
    {
        if (!_bg_map)
            return;

        auto vram = _bg_map->vram();

        if (!vram)
            return;

        const int width = _bg_map->dimensions().width();
        const int index = y * width + x;
        bn::regular_bg_map_cell_info cell(vram.value()[index]);
        cell.set_tile_index(tile_index + _bg_map->tiles_offset());
        vram.value()[index] = cell.cell();
    }

    void process_progress_scene::enter()
    {
        _pop_up.reset();
        _current_cart_infos = flash_context::instance().get_current_cart_infos();
        _progress_index = 3;
        old_progress_index = _progress_index;
        _pending_process_infos_request = request_status::IDLE;

        auto current_process_type = flash_context::instance().get_requested_process_type();

        if (!current_process_type.has_value())
            BN_ERROR("Process type is nullopt");

        process_type type = current_process_type.value();

        if ((type == process_type::DUMPING || type == process_type::BACKUP_SAVE) && !_current_cart_infos.has_value())
            BN_ERROR("Cart infos is nullopt");

        bn::bg_tiles::set_allow_offset(false);

        constexpr int map_width = 32;
        constexpr int map_height = 32;
        constexpr int map_cell_count = map_width * map_height;

        alignas(4) bn::array<bn::regular_bg_map_cell, map_cell_count> decompressed_cells;
        [[maybe_unused]] auto result = process_progress_scene_bg_map_item.decompress(decompressed_cells);

        auto tiles = bn::regular_bg_tiles_items::tiles.create_tiles();
        auto palette = bn::regular_bg_tiles_items::tiles_palette.create_palette();
        _bg_map.emplace(bn::regular_bg_map_ptr::allocate(bn::size(map_width, map_height),
                                                         bn::move(tiles),
                                                         bn::move(palette)));

        auto vram = _bg_map->vram();

        if (!vram.has_value())
            BN_ERROR("BG map VRAM is nullopt");

        const int tiles_offset = _bg_map->tiles_offset();
        const int palette_offset = _bg_map->palette_banks_offset();

        for (int index = 0; index < map_cell_count; ++index)
        {
            bn::regular_bg_map_cell_info cell(decompressed_cells[index]);
            cell.set_tile_index(cell.tile_index() + tiles_offset);
            cell.set_palette_id(cell.palette_id() + palette_offset);
            vram.value()[index] = cell.cell();
        }

        _background.emplace(bn::regular_bg_ptr::create(0, 0, _bg_map.value()));
        _background->set_top_left_position(0, 0);
        _background->set_priority(1);
        bn::bg_tiles::set_allow_offset(true);

        set_content_priority(1);

        auto &process_api = api::process_info_api::instance();
        process_api.start_process(type);
        process_api.request_process_infos();
        _pending_process_infos_request = request_status::PENDING;
    }

    void process_progress_scene::exit()
    {
        _pop_up.reset();

        for (auto &sprite : _sprites)
            sprite.set_visible(false);

        _text_sprites.clear();
        _background.reset();
        _bg_map.reset();
        _current_cart_infos.reset();
        _progress_index = 3;
        old_progress_index = _progress_index;
        _pending_process_infos_request = request_status::IDLE;
        api::process_info_api::instance().request_process_infos_reset();
    }

    void process_progress_scene::update()
    {
        if (_pop_up)
        {
            _pop_up->update();

            if (_pop_up->is_open())
                return;

            _pop_up.reset();
            scene_state_machine::instance().request_scene_state(scene_type::MAIN_MENU);
            return;
        }

        api::process_info_api::instance().update();
        update_screen_infos();
    }

    void process_progress_scene::set_content_priority(int priority)
    {
        if (_background)
            _background->set_priority(priority);

        _text_generator.set_bg_priority(priority);

        for (auto &sprite : _text_sprites)
            sprite.set_bg_priority(priority);

        for (auto &sprite : _sprites)
            sprite.set_bg_priority(priority);
    }

    void process_progress_scene::render()
    {
    }

    scene_type process_progress_scene::get_scene_type()
    {
        return _type;
    }

    void process_progress_scene::set_title(const bn::string_view &title)
    {
        _title = title;
    }
} // namespace openflash
