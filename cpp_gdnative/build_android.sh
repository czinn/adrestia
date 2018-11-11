#!/bin/bash
set -e

source config.sh

# We also use `ndk-build` to build our library. The configs in the jni/ folder
# tell ndk-build to link against the static libraries in godot-cpp-libs/ that
# we generated with `./setup_android.sh`.
ndk-build -j${JOBS}

# We copy the resulting dynamic libraries to bin/ to make them visible to
# Godot.
for dir in libs/*/; do
  platform=$(basename $dir)
  mkdir -p bin/android/$platform
  cp libs/$platform/*.so bin/android/$platform/
  echo "libs/$platform -> bin/android/$platform"
done

echo "Done."
