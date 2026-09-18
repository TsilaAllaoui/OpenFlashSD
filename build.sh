#!/bin/bash

set -e

python3 tools/tmx_to_cpp.py maps
make -j11
mgba-qt openflashsd.gba
