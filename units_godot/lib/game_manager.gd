extends Reference

# Interface for game managers, which accept actions and produce views.

# Performs an action on the underlying game state
func perform_action(action):
  pass

# Starts the game. callback is called with no arguments when the other player
# has selected their resources.
func start_game(callback_obj, callback):
  pass

# Gets the view for the current game state
func get_view():
  pass

# Ends the turn for the user. Callback is called when the other player is done their turn.
func end_turn(callback_obj, callback):
  pass

# Ends the turn. callback is called with the BattleResult.
func simulate_battle(callback_obj, callback):
  pass
