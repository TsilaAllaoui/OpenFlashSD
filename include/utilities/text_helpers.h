#ifndef TEXT_HELPERS_H
#define TEXT_HELPERS_H

#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_string.h"
#include "bn_string_view.h"
#include "bn_vector.h"

namespace openflash::text_helpers
{
    void clear_text(
        bn::ivector<bn::sprite_ptr> &text_sprites);

    void draw_left(
        bn::sprite_text_generator &text_generator,
        bn::string_view text,
        int x,
        int y,
        bn::ivector<bn::sprite_ptr> &text_sprites);

    void draw_centered(
        bn::sprite_text_generator &text_generator,
        bn::string_view text,
        int y,
        bn::ivector<bn::sprite_ptr> &text_sprites);

    void draw_centered_at(
        bn::sprite_text_generator &text_generator,
        bn::string_view text,
        int center_x,
        int y,
        bn::ivector<bn::sprite_ptr> &text_sprites);

    void draw_right(
        bn::sprite_text_generator &text_generator,
        bn::string_view text,
        int x,
        int y,
        bn::ivector<bn::sprite_ptr> &text_sprites);

    bn::string<64> truncate_text(
        bn::string_view text,
        int max_characters);

    void draw_centered_truncated(
        bn::sprite_text_generator &text_generator,
        bn::string_view text,
        int y,
        int max_characters,
        bn::ivector<bn::sprite_ptr> &text_sprites);

    void draw_label_value(
        bn::sprite_text_generator &text_generator,
        bn::string_view label,
        bn::string_view value,
        int left_x,
        int right_x,
        int y,
        bn::ivector<bn::sprite_ptr> &text_sprites,
        int min_spacing = 8);

    void draw_centered_multiline(
        bn::sprite_text_generator &text_generator,
        bn::string_view text,
        int center_x,
        int start_y,
        int line_spacing,
        bn::ivector<bn::sprite_ptr> &text_sprites);
}

#endif // TEXT_HELPERS_H