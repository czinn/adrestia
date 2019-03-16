#!/bin/bash

export SERVER_PORT=16969;

names=(
D33z_Nutz
xXxGetG0m3dxXx
"The Senate"

Dave
Snooper
Mark

Leah
Angela
Michelle

"Bangers and Nash"
"Adrestia SRM"
"A* Strat Finder"

Artanis
"Crazy Ivan"
Monkeylord

Orange_Bird
Tractorm4n
"Red Dot"

TheBigWeeb
ZeroTwo
Madoka

Midona
Raven
Ziz
)

uuids=()

for name in "${names[@]}"; do
  uuid=$(./run.sh ./autofill create "$name" | tail -n1 | cut -d ' ' -f 2)
  echo "Created $name as uuid $uuid"
  uuids+=("$uuid")
done

num_uuids=${#uuids[@]}
while true; do
  index=$(($RANDOM % $num_uuids))
  name="${names[$index]}"
  uuid="${uuids[$index]}"
  ./autofill play "$uuid"
  sleep 30
done
