extends Node

onready var animation_player = $Control/AnimationPlayer
onready var settings_window = $Control/Settings

onready var g = get_node("/root/global")
onready var tech = preload('res://native/tech.gdns')

func _ready():
  get_tree().set_auto_accept_quit(true)
  get_tree().set_quit_on_go_back(true)
  print("OS is %s" % [OS.get_name()])
  animation_player.play("LoadingFade")

func _on_button_play_pressed():
  g.new_ai_game()

func _on_button_settings_pressed():
  get_tree().set_auto_accept_quit(false)
  get_tree().set_quit_on_go_back(false)
  settings_window.visible = true
  animation_player.play("SettingsEnter")

func _on_Settings_BackButton_pressed():
  animation_player.play("SettingsEnter", -1, -1.0, true)
  # TODO: jim: This will wait until ANY animation finishes. May break if we have more animations.
  yield(animation_player, "animation_finished")
  settings_window.visible = false

func _notification(what):
  if what == MainLoop.NOTIFICATION_WM_GO_BACK_REQUEST or what == MainLoop.NOTIFICATION_WM_QUIT_REQUEST:
    if settings_window.visible:
      animation_player.play("SettingsEnter", -1, -1.0, true)
      get_tree().set_quit_on_go_back(true)
    else:
      get_tree().quit()
