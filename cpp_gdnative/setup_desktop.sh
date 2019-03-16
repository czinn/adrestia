#!/bin/bash
set -e

source ./config.sh

# First, we pull in `godot_headers` and `godot-cpp` as submodules.
# `godot_headers` provides the gdnative C api. If you have the godot source
# code, it is pretty much the same as what is located at
# `modules/gdnative/include`. `godot-cpp` is a layer that makes it easier to
# write idiomatic C++ code for gdnative.
#git submodule update --init

# godot-cpp uses a JSON description of the godot API to generate a bunch of
# header files and implementations. Use the local godot executable to generate
# it.
godot --gdnative-generate-json-api godot_headers/api.json

# Build it! The resulting static library will be put in godot-cpp/bin/, and
# will be named something like "libgodot-cpp.linux.64.a".
function build() {
  scons \
    platform=${PLATFORM} \
    headers_dir=../godot_headers \
    use_custom_api_file=yes \
    custom_api_file=../godot_headers/api.json \
    bits=64 \
    -j${JOBS} \
    generate_bindings=yes $@
}
(cd godot-cpp && build -c)
(cd godot-cpp && build)
