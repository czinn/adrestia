#!/bin/bash
set -o allexport
source .env
set +o allexport
cat <(echo "insert into adrestia_rules (game_rules) values ('") <(cat $1 | sed "s/'/''/g") <(echo "');") | psql $DB_CONNECTION_STRING
