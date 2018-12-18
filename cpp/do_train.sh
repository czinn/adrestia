#!/bin/bash

FILE=${1:-"trains.txt"}
OUTPUT="cfr_train/$FILE"
ITERATIONS=${2:-100}

for i in `seq $ITERATIONS`; do
	./train >> $OUTPUT
	printf "."
done

printf "\n"
