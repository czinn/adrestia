#!/bin/bash
set -o allexport
source .env
set +o allexport

MESSAGE=$1
TARGET=${2:-*}

echo "Message: $MESSAGE"
echo "Target:  $TARGET"

read -p "Are you sure you wish to send? [yN]" -n 1 -r
echo    # (optional) move to a new line
if [[ $REPLY =~ ^[Yy]$ ]]
then
	MESSAGE=$(echo $MESSAGE | sed "s/'/''/g")
	echo "insert into adrestia_notifications (target_uuid, message) values ('$TARGET', '$MESSAGE')" | psql "$DB_CONNECTION_STRING"
	echo "Message sent."
else
	echo "Message not sent."
fi
