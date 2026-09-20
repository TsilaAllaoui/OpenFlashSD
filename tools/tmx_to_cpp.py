#!/usr/bin/env python3

import sys
import re
from pathlib import Path

H_FLIP = 0x80000000
V_FLIP = 0x40000000
D_FLIP = 0x20000000

# "auto" chooses the smallest of uncompressed, GBA RLE and GBA LZ77.
# Other accepted values: "none", "run_length", "lz77".
MAP_COMPRESSION = "auto"


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


def cells_to_bytes(cells):
    """Convert 16-bit GBA map cells to their little-endian byte representation."""
    data = bytearray()

    for cell in cells:
        data.append(cell & 0xFF)
        data.append((cell >> 8) & 0xFF)

    return bytes(data)


def gba_run_length_compress(data):
    """
    Encode data in the GBA BIOS RLUnComp format used by Butano's
    bn::compression_type::RUN_LENGTH.

    Header:
        byte 0      = 0x30
        bytes 1..3  = decompressed size (24-bit little-endian)

    Blocks:
        0xxxxxxx : copy (x + 1) literal bytes
        1xxxxxxx : repeat next byte (x + 3) times
    """
    if len(data) > 0xFFFFFF:
        raise RuntimeError("Data is too large for GBA run-length compression")

    result = bytearray((
        0x30,
        len(data) & 0xFF,
        (len(data) >> 8) & 0xFF,
        (len(data) >> 16) & 0xFF,
    ))

    pos = 0
    size = len(data)

    while pos < size:
        # Find a repeated run starting at pos. BIOS RLE supports 3..130 bytes.
        run_length = 1
        while (
            pos + run_length < size and
            run_length < 130 and
            data[pos + run_length] == data[pos]
        ):
            run_length += 1

        if run_length >= 3:
            result.append(0x80 | (run_length - 3))
            result.append(data[pos])
            pos += run_length
            continue

        # Literal block: collect up to 128 bytes, but stop immediately before
        # the next compressible run so that run can be encoded efficiently.
        literal_start = pos
        pos += run_length

        while pos < size and pos - literal_start < 128:
            next_run = 1
            while (
                pos + next_run < size and
                next_run < 130 and
                data[pos + next_run] == data[pos]
            ):
                next_run += 1

            if next_run >= 3:
                break

            if pos - literal_start + next_run > 128:
                break

            pos += next_run

        literal_length = pos - literal_start
        result.append(literal_length - 1)
        result.extend(data[literal_start:pos])

    # Butano requires compressed BG map data to be 4-byte aligned.
    # Padding is ignored by the BIOS decompressor after the output size is met.
    while len(result) % 4:
        result.append(0)

    return bytes(result)



def gba_lz77_compress(data):
    """
    Encode data in the GBA BIOS LZ77 format used by Butano's
    bn::compression_type::LZ77.

    Header:
        byte 0      = 0x10
        bytes 1..3  = decompressed size (24-bit little-endian)

    Payload is arranged in groups of eight tokens. A set flag bit means
    a two-byte back-reference; a clear bit means one literal byte.
    """
    if len(data) > 0xFFFFFF:
        raise RuntimeError("Data is too large for GBA LZ77 compression")

    result = bytearray((
        0x10,
        len(data) & 0xFF,
        (len(data) >> 8) & 0xFF,
        (len(data) >> 16) & 0xFF,
    ))

    pos = 0
    size = len(data)

    while pos < size:
        flags_index = len(result)
        result.append(0)
        flags = 0

        for token_index in range(8):
            if pos >= size:
                break

            best_length = 0
            best_distance = 0
            window_start = max(0, pos - 4096)
            max_length = min(18, size - pos)

            # Prefer nearer matches when two matches have the same length.
            for candidate in range(pos - 1, window_start - 1, -1):
                if data[candidate] != data[pos]:
                    continue

                distance = pos - candidate
                length = 1

                # GBA LZ77 permits overlapping back-references. Once the
                # match reaches the current output position, the pattern
                # repeats every `distance` bytes.
                while length < max_length:
                    source_index = candidate + (length % distance)

                    if data[source_index] != data[pos + length]:
                        break

                    length += 1

                if length >= 3 and length > best_length:
                    best_length = length
                    best_distance = distance

                    if best_length == max_length:
                        break

            if best_length >= 3:
                flags |= 1 << (7 - token_index)

                displacement = best_distance - 1
                result.append(
                    ((best_length - 3) << 4) |
                    ((displacement >> 8) & 0x0F)
                )
                result.append(displacement & 0xFF)
                pos += best_length
            else:
                result.append(data[pos])
                pos += 1

        result[flags_index] = flags

    while len(result) % 4:
        result.append(0)

    return bytes(result)

def bytes_to_halfwords(data):
    """Pack a byte stream into little-endian uint16 values for C++ output."""
    if len(data) % 2:
        data += b"\x00"

    return [
        data[index] | (data[index + 1] << 8)
        for index in range(0, len(data), 2)
    ]


def format_halfwords(values, per_row=8):
    rows = []

    for start in range(0, len(values), per_row):
        row = values[start:start + per_row]
        rows.append(
            "            " +
            ", ".join(f"0x{x:04X}" for x in row)
        )

    return ",\n".join(rows)


def prepare_map_data(cells):
    raw_bytes = cells_to_bytes(cells)

    if MAP_COMPRESSION == "none":
        return cells, "NONE", len(raw_bytes), len(raw_bytes)

    if MAP_COMPRESSION == "run_length":
        compressed = gba_run_length_compress(raw_bytes)
        return bytes_to_halfwords(compressed), "RUN_LENGTH", len(raw_bytes), len(compressed)

    if MAP_COMPRESSION == "lz77":
        compressed = gba_lz77_compress(raw_bytes)
        return bytes_to_halfwords(compressed), "LZ77", len(raw_bytes), len(compressed)

    if MAP_COMPRESSION == "auto":
        candidates = [
            (len(raw_bytes), "NONE", raw_bytes),
        ]

        rle = gba_run_length_compress(raw_bytes)
        candidates.append((len(rle), "RUN_LENGTH", rle))

        lz77 = gba_lz77_compress(raw_bytes)
        candidates.append((len(lz77), "LZ77", lz77))

        stored_size, compression, stored_data = min(
            candidates,
            key=lambda candidate: candidate[0]
        )

        if compression == "NONE":
            return cells, compression, len(raw_bytes), stored_size

        return bytes_to_halfwords(stored_data), compression, len(raw_bytes), stored_size

    raise RuntimeError(f"Unknown MAP_COMPRESSION: {MAP_COMPRESSION}")


def generate_cpp(name, width, height, cells):
    stored_cells, compression, raw_size, stored_size = prepare_map_data(cells)
    cell_text = format_halfwords(stored_cells)
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

#include "bn_compression_type.h"
#include "bn_regular_bg_map_cell.h"
#include "bn_size.h"

namespace openflash
{{
    namespace
    {{
        alignas(4) constexpr bn::regular_bg_map_cell cells[] =
        {{
{cell_text}
        }};
    }}

    const bn::regular_bg_map_item {name}_map_item(
        cells[0],
        bn::size({width}, {height}),
        bn::compression_type::{compression}
    );
}}
"""

    return h, cpp, compression, raw_size, stored_size


def convert_tmx_file(tmx_file, include_dir, src_dir):
    tmx_file = Path(tmx_file)

    firstgid, width, height, values = parse_tmx(tmx_file)

    cells = [
        convert_cell(value, firstgid)
        for value in values
    ]

    name = make_cpp_identifier(tmx_file.stem)

    h, cpp, compression, raw_size, stored_size = generate_cpp(
        name,
        width,
        height,
        cells
    )

    h_file = include_dir / f"{name}.h"
    cpp_file = src_dir / f"{name}.cpp"

    h_file.write_text(h, encoding="utf-8")
    cpp_file.write_text(cpp, encoding="utf-8")

    saved = raw_size - stored_size
    percent = (saved * 100.0 / raw_size) if raw_size else 0.0

    print(f"[OK] {tmx_file}")
    print(f"     -> {h_file}")
    print(f"     -> {cpp_file}")
    print(f"     Map: {width} x {height}")
    print(f"     firstgid: {firstgid}")
    print(f"     compression: {compression}")
    print(f"     data: {raw_size} -> {stored_size} bytes ({percent:.1f}% saved)")
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
    #   python3 tools/tmx_to_cpp.py maps/main_menu_bg.tmx
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
    include_dir = Path(sys.argv[2]) if len(sys.argv) >= 3 else Path("include/generated")
    src_dir = Path(sys.argv[3]) if len(sys.argv) >= 4 else Path("src/generated")

    include_dir.mkdir(parents=True, exist_ok=True)
    src_dir.mkdir(parents=True, exist_ok=True)

    try:
        tmx_files = collect_tmx_files(input_path)

        if not tmx_files:
            print(f"No .tmx files found in: {input_path}")
            sys.exit(0)

        print(f"Found {len(tmx_files)} TMX file(s) in {input_path}")
        print(f"Map compression: {MAP_COMPRESSION}")
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