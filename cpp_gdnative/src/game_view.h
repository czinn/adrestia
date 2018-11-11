#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <game_view.h>

#include "macros.h"

namespace godot {
  class GameView : public godot::GodotScript<Reference>, public OwnerOrPointer<::GameView> {
    GODOT_CLASS(GameView)
   public:
    static const char *resource_path;
    static void _register_methods();

    INTF_NULLABLE
    INTF_TO_JSONABLE
  };
}
