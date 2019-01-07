DROP TABLE IF EXISTS adrestia_accounts;
CREATE TABLE IF NOT EXISTS adrestia_accounts (
	uuid VARCHAR NOT NULL,  -- Random hexy
	user_name VARCHAR NOT NULL,
	tag VARCHAR NOT NULL,
	hash_of_salt_and_password BYTEA NOT NULL,
	salt VARCHAR NOT NULL,
	PRIMARY KEY (uuid)
);
CREATE UNIQUE INDEX IF NOT EXISTS idx_accounts_name_and_tag
	on adrestia_accounts
	(user_name, tag)
;

DROP TABLE IF EXISTS adrestia_match_waiters;
CREATE TABLE IF NOT EXISTS adrestia_match_waiters (
	uuid VARCHAR NOT NULL,  -- Random hexy
	PRIMARY KEY (uuid)
);

-- Player states:
--     -1: Aborted; this person is not coming and the game should be terminated.
--     0: Thinking; this person is deciding on their move.
--     1: Ready; this person has submitted a move.
-- Game activity states:
--     -1: Aborted; Someone aborted and the game cannot continue.
--     0: In progress; this game is in progress.
--     1: Concluded; the game reached its conclusion, with someone winning.
DROP TABLE IF EXISTS adrestia_games;
CREATE TABLE IF NOT EXISTS adrestia_games (
	game_uid VARCHAR NOT NULL,
	creator_uuid VARCHAR NOT NULL, -- Who made this?
	involved_uuids VARCHAR ARRAY NOT NULL,
	activity_state SMALLINT NOT NULL,
	player_states SMALLINT ARRAY NOT NULL,
	player_moves VARCHAR ARRAY,
	game_state VARCHAR,
	PRIMARY KEY (game_uid)
);

DROP TABLE IF EXISTS adrestia_active_users;
CREATE TABLE IF NOT EXISTS adrestia_active_users
( account_uuid VARCHAR NOT NULL
, checkin_time TIMESTAMP NOT NULL
);
CREATE INDEX IF NOT EXISTS idx_adrestia_active_users_account_uuid
	on adrestia_active_users
	(account_uuid)
;
