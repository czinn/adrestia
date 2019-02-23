DROP TABLE IF EXISTS adrestia_accounts CASCADE;
CREATE TABLE IF NOT EXISTS adrestia_accounts (
	uuid VARCHAR NOT NULL,  -- Random hexy
	user_name VARCHAR NOT NULL,
	tag VARCHAR NOT NULL,
	hash_of_salt_and_password BYTEA NOT NULL,
	salt VARCHAR NOT NULL,
	last_login TIMESTAMPTZ,
	last_message TIMESTAMPTZ,
	PRIMARY KEY (uuid)
);
CREATE UNIQUE INDEX IF NOT EXISTS idx_accounts_name_and_tag
	on adrestia_accounts
	(user_name, tag)
;


DROP TABLE IF EXISTS adrestia_match_waiters CASCADE;
CREATE TABLE IF NOT EXISTS adrestia_match_waiters (
	uuid VARCHAR NOT NULL,  -- Random hexy
	selected_books VARCHAR ARRAY NOT NULL,
	PRIMARY KEY (uuid)
);


DROP TABLE IF EXISTS adrestia_rules CASCADE;
CREATE TABLE IF NOT EXISTS adrestia_rules (
	id SERIAL PRIMARY KEY,
	game_rules JSON
);


-- Game activity states:
--     -1: Aborted; Someone aborted and the game cannot continue.
--     0: In progress; this game is in progress.
--     1: Concluded; the game reached its conclusion, with someone winning.
-- Possible winner values:
--    -2: Tie
--    0: Player 0 won
--    1: Player 1 won
DROP TABLE IF EXISTS adrestia_games CASCADE;
CREATE TABLE IF NOT EXISTS adrestia_games (
	game_uid VARCHAR NOT NULL,
	creator_uuid VARCHAR NOT NULL, -- Who made this?
	activity_state SMALLINT NOT NULL,
	game_state JSON,
	last_events JSON,
	winner_id SMALLINT,
	game_rules_id INTEGER REFERENCES adrestia_rules(id),
	PRIMARY KEY (game_uid)
);


-- Player states:
--     -1: Aborted; this person is not coming and the game should be terminated.
--     0: Thinking; this person is deciding on their move.
--     1: Ready; this person has submitted a move.
DROP TABLE IF EXISTS adrestia_players CASCADE;
CREATE TABLE IF NOT EXISTS adrestia_players (
	game_uid VARCHAR REFERENCES adrestia_games(game_uid),
	user_uid VARCHAR REFERENCES adrestia_accounts(uuid),
	player_id SMALLINT NOT NULL, -- 0 or 1
	player_state SMALLINT NOT NULL,
	player_move JSON,
	PRIMARY KEY (game_uid, user_uid)
);


DROP TABLE IF EXISTS adrestia_notifications CASCADE;
CREATE TABLE IF NOT EXISTS adrestia_notifications (
	id SERIAL PRIMARY KEY,
	target_uuid VARCHAR NOT NULL, -- Account uuid or "*"
	message VARCHAR NOT NULL
);
