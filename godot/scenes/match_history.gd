extends Node

onready var g = get_node('/root/global')

onready var match_list = $ui/scroll_container/v_box_container
onready var back_button = $ui/back_button
onready var offline_warning = $ui/offline_warning
onready var template_row = $ui/template_row
onready var game_history_panel = $ui/game_history_panel
onready var game_history = $ui/game_history_panel/game_history
onready var done_button = $ui/game_history_panel/done_button

var games = []
var history_state = null

func _ready():
	back_button.connect('pressed', self, 'on_back_button_pressed')
	done_button.connect('pressed', self, 'on_done_button_pressed')
	g.clear_children(match_list)
	g.network.register_handlers(self, 'on_connected', 'on_disconnected', 'on_out_of_date')
	offline_warning.visible = false
	template_row.visible = false
	history_state = g.GameState.new()
	redraw()

func _notification(what):
	if what == MainLoop.NOTIFICATION_WM_QUIT_REQUEST:
		self.call_deferred('on_back_button_pressed')

func on_get_match_history(response):
	if response.api_code == 200:
		games = response.games
		redraw()

func on_back_button_pressed():
	g.sound.play_sound('button')
	g.scene_loader.goto_scene('title', true)

func on_done_button_pressed():
	g.sound.play_sound('button')
	game_history_panel.visible = false

func on_view_button_pressed(index):
	var game = games[index]
	game_history.view_player_id = game.player_id
	history_state.init_json(g.get_rules(), game.game_state)
	game_history.scroll_container.scroll_vertical = 0
	game_history.state = history_state
	game_history_panel.visible = true

func on_connected():
	g.network.get_match_history(funcref(self, 'on_get_match_history'))
	offline_warning.visible = false

func on_disconnected():
	offline_warning.text = "Offline"
	offline_warning.visible = true

func on_out_of_date():
	offline_warning.text = "Client out of date"
	offline_warning.visible = true

func redraw():
	if match_list == null: return

	g.clear_children(match_list)
	for index in range(len(games)):
		var game = games[index]
		var row = template_row.duplicate()
		var title_label = g.child(row, 'title_label')
		var date_label = g.child(row, 'date_label')
		var view_button = g.child(row, 'view_button')
		view_button.connect('pressed', self, 'on_view_button_pressed', [index])
		var win_text = ''
		if game.winner_id == -2:
			win_text = 'Draw'
		elif game.winner_id == game.player_id:
			win_text = 'Victory'
		else:
			win_text = 'Defeat'
		title_label.bbcode_text = '[b]' + win_text + '[/b] vs. ' + game.opponent_user_name
		print(title_label.bbcode_text)
		date_label.text = game.creation_time.split(' ')[0]
		row.visible = true
		match_list.add_child(row)
