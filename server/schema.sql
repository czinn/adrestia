DROP TABLE IF EXISTS adrestia_accounts CASCADE;
CREATE TABLE IF NOT EXISTS adrestia_accounts (
  uuid VARCHAR NOT NULL,  -- Random hexy
  user_name VARCHAR NOT NULL,
  tag VARCHAR NOT NULL,
  friend_code VARCHAR,
  platform VARCHAR,
  hash_of_salt_and_password BYTEA NOT NULL,
  salt VARCHAR NOT NULL,
  last_login TIMESTAMPTZ,
  debug BOOLEAN NOT NULL DEFAULT FALSE,
  deactivated BOOLEAN NOT NULL DEFAULT FALSE,
  creation_time TIMESTAMPTZ DEFAULT NOW(),
  is_online BOOLEAN NOT NULL DEFAULT FALSE,
  PRIMARY KEY (uuid)
);
CREATE UNIQUE INDEX IF NOT EXISTS idx_accounts_name_and_tag
  ON adrestia_accounts
  (user_name, tag);
CREATE UNIQUE INDEX IF NOT EXISTS idx_accounts_friend_code
  ON adrestia_accounts
  (friend_code);

-- Keeps track of all the IPs an account has ever logged in from, for user
-- uniqueness checking.
DROP TABLE IF EXISTS account_ips CASCADE;
CREATE TABLE IF NOT EXISTS account_ips (
  uuid VARCHAR REFERENCES adrestia_accounts(uuid) NOT NULL,
  ip VARCHAR NOT NULL
);
CREATE UNIQUE INDEX IF NOT EXISTS idx_account_ips_uuid_and_ip ON account_ips (uuid, ip);
CREATE INDEX IF NOT EXISTS idx_account_ip ON account_ips (ip);

DROP TABLE IF EXISTS adrestia_match_waiters CASCADE;
CREATE TABLE IF NOT EXISTS adrestia_match_waiters (
  uuid VARCHAR REFERENCES adrestia_accounts(uuid) NOT NULL,
  selected_books VARCHAR ARRAY NOT NULL,
  target_uuid VARCHAR REFERENCES adrestia_accounts(uuid) DEFAULT '' NOT NULL,
  PRIMARY KEY (uuid)
);
CREATE INDEX IF NOT EXISTS idx_adrestia_match_waiters_uuid
  ON adrestia_match_waiters (uuid);

DROP TABLE IF EXISTS adrestia_rules CASCADE;
CREATE TABLE IF NOT EXISTS adrestia_rules (
  id SERIAL PRIMARY KEY,
  game_rules JSON
);
CREATE INDEX IF NOT EXISTS idx_adrestia_rules_id
  ON adrestia_rules (id);

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
  creator_uuid VARCHAR REFERENCES adrestia_accounts(uuid) NOT NULL, -- Who made this?
  activity_state SMALLINT NOT NULL,
  game_state JSON,
  last_events JSON,
  winner_id SMALLINT,
  game_rules_id INTEGER REFERENCES adrestia_rules(id),
  creation_time TIMESTAMPTZ DEFAULT NOW(),
  PRIMARY KEY (game_uid)
);
CREATE UNIQUE INDEX IF NOT EXISTS idx_adrestia_games_game_uid
  ON adrestia_games (game_uid);
CREATE INDEX IF NOT EXISTS idx_adrestia_games_creator_uuid
  ON adrestia_games (creator_uuid);

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
  last_move_time TIMESTAMPTZ,
  PRIMARY KEY (game_uid, user_uid)
);
CREATE INDEX IF NOT EXISTS idx_adrestia_players_game_uid
  ON adrestia_players (game_uid);
CREATE INDEX IF NOT EXISTS idx_adrestia_players_user_uid
  ON adrestia_players (user_uid);

DROP TABLE IF EXISTS adrestia_notifications CASCADE;
CREATE TABLE IF NOT EXISTS adrestia_notifications (
  id SERIAL PRIMARY KEY,
  target_uuid VARCHAR NOT NULL, -- Account uuid or "*"
  message VARCHAR NOT NULL
);
CREATE INDEX IF NOT EXISTS idx_adrestia_players_game_uid ON adrestia_players (game_uid);

DROP TABLE IF EXISTS adrestia_follows CASCADE;
CREATE TABLE IF NOT EXISTS adrestia_follows (
  uuid1 VARCHAR REFERENCES adrestia_accounts(uuid),
  uuid2 VARCHAR REFERENCES adrestia_accounts(uuid)
);
CREATE UNIQUE INDEX IF NOT EXISTS idx_adrestia_follows_uuid1_uuid2 ON adrestia_follows (uuid1, uuid2);
CREATE INDEX IF NOT EXISTS idx_adrestia_follows_uuid2 ON adrestia_follows (uuid2);

DROP TABLE IF EXISTS challenges CASCADE;
CREATE TABLE IF NOT EXISTS challenges (
  sender_uuid VARCHAR REFERENCES adrestia_accounts(uuid),
  receiver_uuid VARCHAR REFERENCES adrestia_accounts(uuid)
);
CREATE INDEX IF NOT EXISTS idx_challenges_receiver_uuid ON challenges (receiver_uuid);
