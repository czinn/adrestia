#!/bin/bash

# First, we pull in `godot_headers` and `godot-cpp` as submodules.
# `godot_headers` provides the gdnative C api. If you have the godot source
# code, it is pretty much the same as what is located at
# `modules/gdnative/include`. `godot-cpp` is a layer that makes it easier to
# write idiomatic C++ code for gdnative.
git submodule update

# `godot-cpp` doesn't come with a SCons config to build for Android, so we use
# ndk-build directly. We included a config for ndk-build at godot-cpp-jni/.
cp -r godot-cpp-jni godot-cpp/jni

# Now we simply build godot-cpp using ndk-build.
(cd godot-cpp && ndk-build -j$(nproc --ignore=1))

# This should produce some directories under godot-cpp/obj/local/, one for each
# ABI. Each directory will contain two files: `libc++_shared.so` is the C++
# standard library, while `libgodot-cpp.a` is a static library that will need
# to link against. We copy them over to godot-cpp-libs/ for convenience.
for dir in godot-cpp/obj/local/*/; do
  platform=$(basename $dir)
  mkdir -p godot-cpp-libs/$platform
  cp -r godot-cpp/obj/local/$platform/*.{so,a} godot-cpp-libs/$platform/
done

# At this point, the compiled static libraries are ready to use. You don't need
# to repeat this process unless there are changes in godot, godot-cpp, or the
# Android NDK.