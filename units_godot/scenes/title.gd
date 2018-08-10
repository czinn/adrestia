extends Node

onready var g = get_node("/root/global")
onready var tech = preload('res://native/tech.gdns')
var in_settings

func _ready():
  get_tree().set_auto_accept_quit(true)
  get_tree().set_quit_on_go_back(true)
  print("OS is %s" % [OS.get_name()])
  $AnimationPlayer.play("LoadingFade")
  in_settings = false

  var tech1 = tech.new()
  tech1.init(5, 3, 2)
  var tech2 = tech.new()
  tech2.init(1, 1, 1)
  tech1.add(tech2)
  print(tech1.as_json().result)

func _on_button_play_pressed():
  if (!in_settings):
    g.new_ai_game()

func _on_button_settings_pressed():
  if (!in_settings):
    get_tree().set_auto_accept_quit(false)
    get_tree().set_quit_on_go_back(false)
    $AnimationPlayer.play("SettingsLoad")
    in_settings = true
    
func _notification(what):
  if what == MainLoop.NOTIFICATION_WM_GO_BACK_REQUEST or what == MainLoop.NOTIFICATION_WM_QUIT_REQUEST:
    if in_settings:
      $AnimationPlayer.play("SettingsLoad", -1, -1.0, true)
      in_settings = false
      get_tree().set_quit_on_go_back(true)
    else:
      get_tree().quit()
