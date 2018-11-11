## Building C++ gdnative scripts

### Prereqs

For desktop, you need SCons.

For Android, you need to have the Android NDK installed, and ndk-build (which
should be the in the root directory of the NDK) should be in your PATH.

You will need to build for desktop before you build for Android, since building
for desktop generates some headers in `godot-cpp` based on the Godot API.

### Desktop

Run `./setup_desktop.sh` to build godot-cpp, then run `./build_desktop.sh` to
build our code in src/.

### Android

Run `./setup_android.sh` to build godot-cpp for Android, then run
`./build_android.sh` to build our code for Android.

### Documentation

All scripts are commented.
