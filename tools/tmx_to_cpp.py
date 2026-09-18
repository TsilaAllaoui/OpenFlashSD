#!/usr/bin/env python3

import sys
import re
from pathlib import Path

H_FLIP = 0x80000000
V_FLIP = 0x40000000
D_FLIP = 0x20000000


def parse_tmx(filename):
    text = Path(filename).read_text(encoding="utf-8")

    m = re.search(r'<tileset\s+firstgid="(\d+)"', text)
    if not m:
        raise RuntimeError(f"{filename}: firstgid not found")

    firstgid = int(m.group(1))

    m = re.search(
        r'<layer[^>]*width="(\d+)"[^>]*height="(\d+)"',
        text
    )

    if not m:
        raise RuntimeError(f"{filename}: layer dimensions not found")

    width = int(m.group(1))
    height = int(m.group(2))

    m = re.search(
        r'<data\s+encoding="csv">(.*?)</data>',
        text,
        re.DOTALL
    )

    if not m:
        raise RuntimeError(f"{filename}: CSV data not found")

    csv_data = m.group(1)

    values = [
        int(x)
        for x in re.findall(r'\d+', csv_data)
    ]

    if len(values) != width * height:
        raise RuntimeError(
            f"{filename}: expected {width * height} cells, got {len(values)}"
        )

    return firstgid, width, height, values


def convert_cell(value, firstgid):
    hflip = bool(value & H_FLIP)
    vflip = bool(value & V_FLIP)

    if value & D_FLIP:
        raise RuntimeError(
            f"Diagonal flip found: 0x{value:08X}"
        )

    gid = value & 0x0FFFFFFF

    if gid == 0:
        return 0

    tile = gid - firstgid

    if tile < 0:
        raise RuntimeError(
            f"Invalid GID {gid}, firstgid={firstgid}"
        )

    cell = tile & 0x03FF

    if hflip:
        cell |= 0x0400

    if vflip:
        cell |= 0x0800

    return cell


def make_cpp_identifier(name):
    name = re.sub(r'[^a-zA-Z0-9_]', '_', name)

    if not name:
        raise RuntimeError("Empty generated C++ identifier")

    if name[0].isdigit():
        name = "_" + name

    return name


def generate_cpp(name, width, height, cells):
    values = []

    for y in range(height):
        row = cells[y * width:(y + 1) * width]

        values.append(
            "            " +
            ", ".join(f"0x{x:04X}" for x in row)
        )

    cell_text = ",\n".join(values)
    guard = f"{name.upper()}_H"

    h = f"""#ifndef {guard}
#define {guard}

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


def convert_tmx_file(tmx_file, include_dir, src_dir):
    tmx_file = Path(tmx_file)

    firstgid, width, height, values = parse_tmx(tmx_file)

    cells = [
        convert_cell(value, firstgid)
        for value in values
    ]

    name = make_cpp_identifier(tmx_file.stem)

    h, cpp = generate_cpp(name, width, height, cells)

    h_file = include_dir / f"{name}.h"
    cpp_file = src_dir / f"{name}.cpp"

    h_file.write_text(h, encoding="utf-8")
    cpp_file.write_text(cpp, encoding="utf-8")

    print(f"[OK] {tmx_file}")
    print(f"     -> {h_file}")
    print(f"     -> {cpp_file}")
    print(f"     Map: {width} x {height}")
    print(f"     firstgid: {firstgid}")
    print()


def collect_tmx_files(input_path):
    input_path = Path(input_path)

    if input_path.is_file():
        if input_path.suffix.lower() != ".tmx":
            raise RuntimeError(f"Not a .tmx file: {input_path}")

        return [input_path]

    if input_path.is_dir():
        return sorted(input_path.rglob("*.tmx"))

    raise RuntimeError(f"Input path does not exist: {input_path}")


def main():
    # Backwards-compatible single file:
    #   python3 tools/tmx_to_cpp.py maps/tile_maps.tmx
    #
    # Convert every TMX in a folder recursively:
    #   python3 tools/tmx_to_cpp.py maps
    #
    # Custom output directories:
    #   python3 tools/tmx_to_cpp.py maps include src

    if len(sys.argv) > 4:
        print(
            "Usage:\n"
            "  python3 tmx_to_cpp.py <tmx-file-or-folder> [include-dir] [src-dir]"
        )
        sys.exit(1)

    input_path = Path(sys.argv[1]) if len(sys.argv) >= 2 else Path("maps")
    include_dir = Path(sys.argv[2]) if len(sys.argv) >= 3 else Path("include")
    src_dir = Path(sys.argv[3]) if len(sys.argv) >= 4 else Path("src")

    include_dir.mkdir(parents=True, exist_ok=True)
    src_dir.mkdir(parents=True, exist_ok=True)

    try:
        tmx_files = collect_tmx_files(input_path)

        if not tmx_files:
            print(f"No .tmx files found in: {input_path}")
            sys.exit(0)

        print(f"Found {len(tmx_files)} TMX file(s) in {input_path}")
        print()

        failed = []

        for tmx_file in tmx_files:
            try:
                convert_tmx_file(
                    tmx_file,
                    include_dir,
                    src_dir
                )
            except Exception as exc:
                failed.append((tmx_file, exc))
                print(f"[ERROR] {tmx_file}: {exc}", file=sys.stderr)
                print(file=sys.stderr)

        if failed:
            print(
                f"Failed to convert {len(failed)} of "
                f"{len(tmx_files)} TMX file(s).",
                file=sys.stderr
            )
            sys.exit(1)

        print(
            f"Successfully converted {len(tmx_files)} TMX file(s)."
        )

    except Exception as exc:
        print(f"Error: {exc}", file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()