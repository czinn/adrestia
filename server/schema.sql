CREATE TABLE IF NOT EXISTS adrestia_accounts (
    uuid VARCHAR NOT NULL,
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
