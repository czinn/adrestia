#!/bin/bash

winspells=$(mktemp)
losespells=$(mktemp)
cat games.json | jq 'select(.winners == [0]) | .history | map(.[0]) | .[] | .[]' -c > $winspells
cat games.json | jq 'select(.winners == [1]) | .history | map(.[1]) | .[] | .[]' -c > $winspells

cat games.json | jq 'select(.winners == [0]) | .history | map(.[1]) | .[] | .[]' -c > $losespells
cat games.json | jq 'select(.winners == [1]) | .history | map(.[0]) | .[] | .[]' -c > $losespells

echo "Winning spells"
cat $winspells | sort | uniq -c | sort -bnr | head -n 20
echo
echo "Losing spells"
cat $losespells | sort | uniq -c | sort -bnr | head -n 20

rm $winspells
rm $losespells

echo
echo

winbooks=$(mktemp)
losebooks=$(mktemp)
cat games.json | jq 'select(.winners == [0]) | .players[0].books | .[]' -c > $winbooks
cat games.json | jq 'select(.winners == [1]) | .players[1].books | .[]' -c > $winbooks

cat games.json | jq 'select(.winners == [0]) | .players[1].books | .[]' -c > $losebooks
cat games.json | jq 'select(.winners == [1]) | .players[0].books | .[]' -c > $losebooks

echo "Winning books"
cat $winbooks | sort | uniq -c | sort -bnr
echo
echo "Losing books"
cat $losebooks | sort | uniq -c | sort -bnr

rm $winbooks
rm $losebooks
