#!/bin/bash

# Does not detect when a NEW asset is created until a build is triggered by
# saving an existing asset.
./build.sh
while inotifywait -e close_write assets/{*.svg,**/*.svg} >/dev/null 2>/dev/null || true; do
  ./build.sh
done
