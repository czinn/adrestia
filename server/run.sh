#!/bin/bash
if [[ "$#" -lt 1 ]]; then
	echo "Use like: ./run.sh server"
	exit 1
fi
set -o allexport
[[ -f .env ]] && source .env || echo "Warning: .env not found"
set +o allexport
./$@
