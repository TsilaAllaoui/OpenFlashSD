#!/usr/bin/env python3

import sys
import re

H_FLIP = 0x80000000
V_FLIP = 0x40000000
D_FLIP = 0x20000000

# Tiled GID transformation bits:
# H = 0x80000000
# V = 0x40000000
# D = 0x20000000

def parse_tmx(filename):
    text = open(filename, "r", encoding="utf-8").read()

    # firstgid
    m = re.search(r'<tileset\s+firstgid="(\d+)"', text)
    if not m:
        raise RuntimeError("firstgid not found")

    firstgid = int(m.group(1))

    # map dimensions
    m = re.search(
        r'<layer[^>]*width="(\d+)"[^>]*height="(\d+)"',
        text
    )

    if not m:
        raise RuntimeError("layer dimensions not found")

    width = int(m.group(1))
    height = int(m.group(2))

    # CSV data
    m = re.search(
        r'<data\s+encoding="csv">(.*?)</data>',
        text,
        re.DOTALL
    )

    if not m:
        raise RuntimeError("CSV data not found")

    csv_data = m.group(1)

    # Extract all decimal numbers
    values = [
        int(x)
        for x in re.findall(r'\d+', csv_data)
    ]

    if len(values) != width * height:
        raise RuntimeError(
            f"Expected {width * height} cells, got {len(values)}"
        )

    return firstgid, width, height, values


def convert_cell(value, firstgid):
    # Tiled transformation flags
    hflip = bool(value & H_FLIP)
    vflip = bool(value & V_FLIP)

    # We don't support diagonal transformations yet.
    if value & D_FLIP:
        raise RuntimeError(
            f"Diagonal flip found: 0x{value:08X}"
        )

    # Remove transformation bits.
    gid = value & 0x0FFFFFFF

    # Empty tile
    if gid == 0:
        return 0

    # Convert Tiled GID -> local tileset index.
    tile = gid - firstgid

    if tile < 0:
        raise RuntimeError(
            f"Invalid GID {gid}, firstgid={firstgid}"
        )

    # GBA regular BG map cell:
    #
    # bits 0-9   = tile index
    # bit 10     = horizontal flip
    # bit 11     = vertical flip
    #
    cell = tile & 0x03FF

    if hflip:
        cell |= 0x0400

    if vflip:
        cell |= 0x0800

    return cell


def generate_cpp(name, width, height, cells):
    values = []

    for y in range(height):
        row = cells[y * width:(y + 1) * width]

        values.append(
            "            " +
            ", ".join(f"0x{x:04X}" for x in row)
        )

    cell_text = ",\n".join(values)

    h = f"""#ifndef {name.upper()}_H
#define {name.upper()}_H

#include "bn_regular_bg_map_item.h"

namespace openflash
{{
    extern const bn::regular_bg_map_item {name}_map_item;
}}

#endif
"""

    cpp = f"""#include "{name}.h"

#include "bn_regular_bg_map_cell.h"
#include "bn_size.h"

namespace openflash
{{
    namespace
    {{
        constexpr bn::regular_bg_map_cell cells[] =
        {{
{cell_text}
        }};
    }}

    const bn::regular_bg_map_item {name}_map_item(
        cells[0],
        bn::size({width}, {height})
    );
}}
"""

    return h, cpp


def main():
    if len(sys.argv) != 2:
        print("Usage: python3 tiled_to_cpp.py file.tmx")
        sys.exit(1)

    tmx_file = sys.argv[1]

    firstgid, width, height, values = parse_tmx(tmx_file)

    cells = [
        convert_cell(value, firstgid)
        for value in values
    ]

    name = tmx_file.rsplit("/", 1)[-1]
    name = name.rsplit(".", 1)[0]

    h, cpp = generate_cpp(name, width, height, cells)

    h_file = "include/" + name + ".h"
    cpp_file = "src/" + name + ".cpp"

    open(h_file, "w", encoding="utf-8").write(h)
    open(cpp_file, "w", encoding="utf-8").write(cpp)

    print(f"Generated {h_file}")
    print(f"Generated {cpp_file}")
    print()
    print(f"Map: {width} x {height}")
    print(f"firstgid: {firstgid}")


if __name__ == "__main__":
    main()