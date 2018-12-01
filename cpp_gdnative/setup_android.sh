#!/bin/bash
set -e

source ./config.sh

# `godot-cpp` doesn't come with a SCons config to build for Android, so we use
# ndk-build directly. We included a config for ndk-build at godot-cpp-jni/.
cp -r godot-cpp-jni godot-cpp/jni

# Now we simply build godot-cpp using ndk-build.
(cd godot-cpp && ndk-build -j${JOBS})

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
