-- Number of multiplayer games with at least two turns
-- total_plays is number of games from user perspective (game counts twice if two players play it)
-- total_games is actual number of distinct games containing at least one real user
select count(*) as total_plays, count(distinct g.game_uid) as total_games
from adrestia_accounts a
  inner join adrestia_players p on a.uuid = p.user_uid
  inner join adrestia_games g on p.game_uid = g.game_uid
where a.debug = false
  and json_array_length(game_state -> 'history') >= 2;

-- Average game length among multiplayer games with at least two turns
select
  avg(turns) as average_multiplayer_turns,
  avg(duration) as average_multiplayer_duration
from
select
  max(json_array_length(game_state -> 'history')) as turns,
  max(last_move_time) - min(g.creation_time) as duration
from adrestia_accounts a
  inner join adrestia_players p on a.uuid = p.user_uid
  inner join adrestia_games g on p.game_uid = g.game_uid
where a.debug = false
  and json_array_length(game_state -> 'history') >= 2
  and last_move_time != null
group by g.game_uid;

-- Average single-player game length
select avg(json_array_length(game_state -> 'history')) as average_singleplayer_turns
from adrestia_single_player_games g
  inner join adrestia_accounts a on a.uuid = g.user_uid
where a.debug = false;
