#!/bin/bash

set -e

python3 tools/tmx_to_cpp.py maps/tile_maps.tmx
make -j11
mgba-qt openflashsd.gba
