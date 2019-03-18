#!/bin/bash
# Replaces the database with a backup produced by [dump.sh].
set -e
set -o allexport
source .env
set +o allexport
if [[ "$#" -ne 1 ]]; then
	echo "Usage: $0 backup.sql"
	exit 1
fi
if [[ ! -f $1 ]]; then
	echo "'$1' does not seem to be a file"
	exit 1
fi
echo "We're gonna fuck up $DB_NAME."
sudo -u postgres dropdb $DB_NAME
sudo -u postgres createdb $DB_NAME
psql "$DB_CONNECTION_STRING" < $1
