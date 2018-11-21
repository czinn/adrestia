#!/bin/bash
set -e

source config.sh

# We also use `ndk-build` to build our library. The configs in the jni/ folder
# tell ndk-build to link against the static libraries in godot-cpp-libs/ that
# we generated with `./setup_android.sh`.

# TODO: jim: This currently has a build warning that occurs many times owing to
# the sketchy way I defined Forwarder in macros.h. It still compiles fine, but
# maybe fix this some time.

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
