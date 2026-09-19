#include "text_helpers.h"

#include "bn_algorithm.h"

namespace openflash::text_helpers
{
    void clear_text(
        bn::ivector<bn::sprite_ptr>& text_sprites)
    {
        text_sprites.clear();
    }


    void draw_left(
        bn::sprite_text_generator& text_generator,
        bn::string_view text,
        int x,
        int y,
        bn::ivector<bn::sprite_ptr>& text_sprites)
    {
        text_generator.set_left_alignment();

        text_generator.generate(
            x,
            y,
            text,
            text_sprites);
    }


    void draw_centered(
        bn::sprite_text_generator& text_generator,
        bn::string_view text,
        int y,
        bn::ivector<bn::sprite_ptr>& text_sprites)
    {
        text_generator.set_center_alignment();

        text_generator.generate(
            0,
            y,
            text,
            text_sprites);
    }


    void draw_centered_at(
        bn::sprite_text_generator& text_generator,
        bn::string_view text,
        int center_x,
        int y,
        bn::ivector<bn::sprite_ptr>& text_sprites)
    {
        text_generator.set_center_alignment();

        text_generator.generate(
            center_x,
            y,
            text,
            text_sprites);
    }


    void draw_right(
        bn::sprite_text_generator& text_generator,
        bn::string_view text,
        int x,
        int y,
        bn::ivector<bn::sprite_ptr>& text_sprites)
    {
        text_generator.set_right_alignment();

        text_generator.generate(
            x,
            y,
            text,
            text_sprites);
    }


    bn::string<64> truncate_text(
        bn::string_view text,
        int max_characters)
    {
        bn::string<64> result;

        int limit = bn::min(max_characters, 64);

        if(text.size() <= limit)
        {
            result = text;
            return result;
        }

        if(limit <= 3)
        {
            for(int index = 0; index < limit; ++index)
            {
                result.push_back('.');
            }

            return result;
        }

        for(int index = 0; index < limit - 3; ++index)
        {
            result.push_back(text[index]);
        }

        result += "...";

        return result;
    }


    void draw_centered_truncated(
        bn::sprite_text_generator& text_generator,
        bn::string_view text,
        int y,
        int max_characters,
        bn::ivector<bn::sprite_ptr>& text_sprites)
    {
        bn::string<64> display_text =
            truncate_text(text, max_characters);

        draw_centered(
            text_generator,
            display_text,
            y,
            text_sprites);
    }


    void draw_label_value(
        bn::sprite_text_generator& text_generator,
        bn::string_view label,
        bn::string_view value,
        int left_x,
        int right_x,
        int y,
        bn::ivector<bn::sprite_ptr>& text_sprites)
    {
        text_generator.set_left_alignment();

        text_generator.generate(
            left_x,
            y,
            label,
            text_sprites);

        text_generator.set_right_alignment();

        text_generator.generate(
            right_x,
            y,
            value,
            text_sprites);
    }


    void draw_centered_multiline(
        bn::sprite_text_generator& text_generator,
        bn::string_view text,
        int center_x,
        int start_y,
        int line_spacing,
        bn::ivector<bn::sprite_ptr>& text_sprites)
    {
        text_generator.set_center_alignment();

        int line_start = 0;
        int line_index = 0;

        for(int index = 0; index <= text.size(); ++index)
        {
            bool end_of_text =
                index == text.size();

            bool new_line =
                !end_of_text &&
                text[index] == '\n';

            if(end_of_text || new_line)
            {
                bn::string_view line =
                    text.substr(
                        line_start,
                        index - line_start);

                text_generator.generate(
                    center_x,
                    start_y + line_index * line_spacing,
                    line,
                    text_sprites);

                ++line_index;
                line_start = index + 1;
            }
        }
    }
}