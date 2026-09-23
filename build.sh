#!/bin/bash

# stop on error
set -e

# 1. Mandatory check for map conversion
if [ -n "$1" ]; then
    # convert maps to code
    python3 tools/tmx_to_cpp.py maps
fi

# # 2. Build configuration logic
# if [ -z "$2" ]; then
#     # Simple make -j11 when no second arg is given
#     make -j11
# else
    # Fallback/Debug build if a second arg is present
    make -j11 OPENFLASH_DEBUG=1
# fi

# run emulator
mgba-qt openflashsd.gba