#!/bin/bash

# stop on error
set -e

# create maps
if [ -n "$1" ]; then
    # convert maps to code
    python3 tools/tmx_to_cpp.py maps
fi

# build
make -j$(nproc) OPENFLASH_MOCK=1 OPENFLASH_DEBUG=0

# run emulator
mgba-qt openflashsd.gba