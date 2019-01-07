#!/bin/bash
# Runs program with additional environment variables from .env
set -o allexport
source .env
set +o allexport
./$@
