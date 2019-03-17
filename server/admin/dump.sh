#!/bin/bash
# Dumps database to a file named with the current time.
set -o allexport
source .env
set +o allexport

out_file="$(date +%F-%T).sql"
# -O: No owner information; whoever does the backup owns everything.
sudo -u postgres pg_dump -O $DB_NAME > $out_file
echo "Dumped database '$DB_NAME' to $out_file"
