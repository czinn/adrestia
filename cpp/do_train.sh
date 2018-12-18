#!/bin/bash

FILE=${1:-"trains.txt"}
OUTPUT="cfr_train/$FILE"
ITERATIONS=${2:-100}

./train $ITERATIONS >> $OUTPUT
