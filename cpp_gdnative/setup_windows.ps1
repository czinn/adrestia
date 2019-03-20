git submodule update --init

Push-Location godot-cpp
    git checkout -- .
    git clean -fx
Pop-Location

godot --gdnative-generate-json-api godot-cpp/godot_api.json

(Get-Content .\godot-cpp\godot_api.json).replace("typename", "type_name") | Set-Content .\godot-cpp\godot_api.json

Push-Location godot-cpp
    scons platform=windows headers_dir=../godot_headers -j4 generate_bindings=yes
Pop-Location