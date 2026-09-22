#include "bn_core.h"
#include "bn_array.h"
#include "bn_keypad.h"
#include "bn_random.h"
#include "bn_display.h"
#include "bn_bg_tiles.h"
#include "bn_bg_palette_ptr.h"
#include "bn_regular_bg_item.h"
#include "bn_sprite_items_info.h"
#include "bn_sprite_items_save.h"
#include "bn_sprite_items_arrow.h"
#include "bn_regular_bg_map_ptr.h"
#include "bn_sprite_items_sdcard.h"
#include "bn_sprite_items_gbacart.h"
#include "bn_regular_bg_tiles_ptr.h"
#include "bn_regular_bg_map_cell_info.h"
#include "bn_regular_bg_tiles_items_tiles.h"

#include "file_entry.h"
#include "api/cart_api.h"
#include "flash_context.h"
#include "string_helpers.h"
#include "flash_screen_bg.h"
#include "api/rom_info_api.h"
#include "utilities/pop_up.h"
#include "api/process_info_api.h"
#include "scene_state_machine.h"
#include "process_progress_scene.h"
#include "utilities/text_helpers.h"
#include "process_progress_scene_bg.h"
#include "common_variable_8x16_sprite_font.h"

namespace openflash
{
    process_progress_scene::process_progress_scene()
        : _type(scene_type::PROCESS_PROGRESS),
          _background(),
          _bg_map(),
          _pop_up_bg(),
          _text_generator(
              bn::sprite_text_generator(
                  common::variable_8x16_sprite_font)),
          _sprites(),
          _progress_index(3),
          old_progress_index(_progress_index)
    {
        _sprites.emplace_back(bn::sprite_items::arrow.create_sprite(5, -10));

        _sprites.emplace_back(bn::sprite_items::info.create_sprite(-bn::display::width() / 2 + 30,
                                                                   60));

        for (auto &sprite : _sprites)
            sprite.set_visible(false);

        old_progress_index = _progress_index;
    }

    void process_progress_scene::update_screen_infos()
    {
        auto current_process_infos = api::process_info_api::instance().get_current_process_infos();

        if (current_process_infos->progress >= 100)
        {
            for (auto &sprite : _text_sprites)
                sprite.set_bg_priority(1);
            for (auto &sprite : _sprites)
                sprite.set_bg_priority(1);
            _background->set_priority(1);

            bn::string<max_character_count> pop_up_header_text = current_process_infos->status == process_status::READING ? "Dump" : "Flash";
            pop_up_header_text += " complete!";
            pop_up popup(pop_up_header_text, true);
            popup.render();
            popup.update();
            scene_state_machine::instance().request_scene_state(scene_type::MAIN_MENU);
        }

        // dynamic tile update for progress
        if (old_progress_index != current_process_infos->progress && current_process_infos->progress % 5 == 0 && current_process_infos->progress > 0)
        {
            old_progress_index = current_process_infos->progress;
            set_tile(_progress_index++, 13, 63);
        }

        _text_sprites.clear();

        // Header
        text_helpers::draw_centered(
            _text_generator,
            _title,
            screen_top + 12,
            _text_sprites);

        if (_current_cart_infos.has_value())
        {
            bn::string<max_character_count> header_text;
            if (current_process_infos->type == process_type::DUMPING)
            {
                header_text = "Path: \"/dump/" +bn::string<32>(_current_cart_infos->cart_rom_infos.name) + ".gba\"";
            }
            else
            {
                auto current_rom_infos = flash_context::instance().get_current_rom_infos();
                header_text = "Path: \"" + current_rom_infos->file_path + "\"";
            }

            text_helpers::draw_centered_at(_text_generator,
                                           text_helpers::truncate_text(header_text, 32),
                                           0,
                                           -48,
                                           _text_sprites);
        }

        // Warning
        text_helpers::draw_centered_at(_text_generator,
                                       "DO NOT POWER OFF CONSOLE",
                                       10,
                                       60,
                                       _text_sprites);

        // Process infos
        text_helpers::draw_centered_at(_text_generator,
                                       bn::to_string<32>(
                                           current_process_infos->progress) +
                                           "%",
                                       85,
                                       27,
                                       _text_sprites);

        // Status
        text_helpers::draw_centered_at(_text_generator,
                                       bn::string<32>("Status: ") +
                                           (current_process_infos->status ==
                                                    process_status::WRITING
                                                ? "WRITING CART"
                                                : "READING CART"),
                                       0,
                                       15,
                                       _text_sprites);

        // Elapsed time
        text_helpers::draw_centered_at(_text_generator,
                                       bn::string<32>("Elapsed: ") +
                                           bn::to_string<32>(
                                               current_process_infos
                                                   ->elapsed_time.minutes) +
                                           ":" +
                                           bn::to_string<32>(
                                               current_process_infos
                                                   ->elapsed_time.seconds),
                                       -bn::display::width() / 2 + 50,
                                       40,
                                       _text_sprites);

        // Speed
        text_helpers::draw_centered_at(_text_generator,
                                       bn::string<32>("Speed: ") +
                                           bn::to_string<32>(
                                               current_process_infos->speed) +
                                           "KiB/s",
                                       60,
                                       40,
                                       _text_sprites);

        for (auto &sprite : _sprites)
        {
            sprite.set_visible(true);
            sprite.set_bg_priority(0);
        }

        for (auto &sprite : _text_sprites)
            sprite.set_bg_priority(0);
    }

    void process_progress_scene::set_tile(int x,
                                          int y,
                                          int tile_index)
    {
        if (!_bg_map)
            return;

        auto vram = _bg_map->vram();

        if (!vram)
            return;

        const int width = _bg_map->dimensions().width();

        const int index = y * width + x;

        bn::regular_bg_map_cell_info cell(vram.value()[index]);

        cell.set_tile_index(tile_index +
                            _bg_map->tiles_offset());

        vram.value()[index] = cell.cell();
    }

    void process_progress_scene::enter()
    {
        bn::bg_tiles::set_allow_offset(false);

        constexpr int map_width = 32;
        constexpr int map_height = 32;
        constexpr int map_cell_count = map_width * map_height;

        // decompress bg tile map
        alignas(4) bn::array<bn::regular_bg_map_cell, map_cell_count> decompressed_cells;

        [[maybe_unused]]
        auto result = process_progress_scene_bg_map_item.decompress(decompressed_cells);

        // recreate bg tiles and palette
        auto tiles = bn::regular_bg_tiles_items::tiles.create_tiles();
        auto palette = bn::regular_bg_tiles_items::tiles_palette.create_palette();

        // allocationg vram map
        _bg_map.emplace(bn::regular_bg_map_ptr::allocate(bn::size(map_width,
                                                                  map_height),
                                                         bn::move(tiles),
                                                         bn::move(palette)));

        // copy decompressed map cells into the allocated vram map
        auto vram = _bg_map->vram();
        BN_ASSERT(vram.has_value());

        const int tiles_offset = _bg_map->tiles_offset();
        const int palette_offset = _bg_map->palette_banks_offset();
        for (int index = 0; index < map_cell_count; ++index)
        {
            bn::regular_bg_map_cell_info cell(decompressed_cells[index]);

            cell.set_tile_index(cell.tile_index() +
                                tiles_offset);

            cell.set_palette_id(cell.palette_id() +
                                palette_offset);

            vram.value()[index] = cell.cell();
        }

        // create the actual bg from writable map.
        _background.emplace(bn::regular_bg_ptr::create(0,
                                                       0,
                                                       _bg_map.value()));

        _background->set_top_left_position(0,
                                           0);

        _background->set_priority(0);

        bn::bg_tiles::set_allow_offset(true);

        _current_cart_infos = api::cart_api::instance().get_current_cart_infos();

        update_screen_infos();
    }

    void process_progress_scene::exit()
    {
        for (auto &sprite : _sprites)
            sprite.set_visible(false);
        _text_sprites.clear();
        _background.reset();
        _bg_map.reset();
        _progress_index = 3;
        old_progress_index = _progress_index;
        
        // placeholder, should be replaced by server saying that process is done and reset everything
        api::process_info_api::instance().reset();
    }

    void process_progress_scene::update()
    {
        update_screen_infos();
    }

    void process_progress_scene::render()
    {
    }

    scene_type process_progress_scene::get_scene_type()
    {
        return _type;
    }

    void process_progress_scene::set_title(
        const bn::string_view &title)
    {
        _title = title;
    }
}