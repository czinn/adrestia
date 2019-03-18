#!/bin/bash
set -e

source ./config.sh

scons platform=${PLATFORM} -j${JOBS}

# scons platform=windows headers=../godot_headers generate_bindings=yes
