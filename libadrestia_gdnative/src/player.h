#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <player.h>

#include "macros.h"

namespace godot {
  class Player : public godot::GodotScript<Reference> {
    GODOT_CLASS(Player)
   public:
    static void _register_methods();
    ::Player *_player;
    Ref<Reference> _owner;

    void set(::Player *p, Reference *o);
    String get_id() const;
    INTF_TO_JSONABLE
  };
}
