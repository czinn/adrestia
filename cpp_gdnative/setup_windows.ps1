git submodule update --init

godot --gdnative-generate-json-api godot-cpp/godot_api.json

Push-Location godot-cpp
    scons platform=windows headers=../godot_headers generate_bindings=yes
Pop-Location