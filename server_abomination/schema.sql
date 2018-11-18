CREATE TABLE adrestia_accounts (
    account_name VARCHAR(255) NOT NULL,
    hash_of_salt_and_password BYTEA NOT NULL,
    salt VARCHAR(255) NOT NULL,
    PRIMARY KEY (account_name)
)
;


