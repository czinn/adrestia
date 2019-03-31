extends VBoxContainer

signal increment
signal decrement

onready var counter = $Counter

func set_total(t):
  counter.set_total(t)

func set_value(v):
  counter.set_value(v)

func set_color(c):
  counter.fill_color = c
  counter.update()

func _on_PlusButton_pressed():
  emit_signal("increment")

func _on_MinusButton_pressed():
  emit_signal("decrement")
