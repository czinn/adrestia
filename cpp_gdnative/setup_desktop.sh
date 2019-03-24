#!/bin/bash
set -e

source ./config.sh

# First, we pull in `godot_headers` and `godot-cpp` as submodules.
# `godot_headers` provides the gdnative C api. If you have the godot source
# code, it is pretty much the same as what is located at
# `modules/gdnative/include`. `godot-cpp` is a layer that makes it easier to
# write idiomatic C++ code for gdnative.
git submodule update --init

(cd godot-cpp && git checkout -- . && git clean -fx)

# godot-cpp uses a JSON description of the godot API to generate a bunch of
# header files and implementations. Use the local godot executable to generate
# it.
godot --gdnative-generate-json-api godot-cpp/godot_api.json

# see https://github.com/godotengine/godot/issues/25006
sed -i.bak 's/typename/type_name/g' godot-cpp/godot_api.json && rm godot-cpp/godot_api.json

# Build it! The resulting static library will be put in godot-cpp/bin/, and
# will be named something like "libgodot-cpp.linux.64.a".
(cd godot-cpp && scons platform=${PLATFORM} headers_dir=../godot_headers generate_bindings=yes -c)
(cd godot-cpp && scons platform=${PLATFORM} headers_dir=../godot_headers -j${JOBS} generate_bindings=yes)
