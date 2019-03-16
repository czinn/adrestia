#!/bin/bash
set -o allexport
source .env
set +o allexport

psql "$DB_CONNECTION_STRING" $@
