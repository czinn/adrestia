#!/bin/bash
set -e

export PLATFORM=linux
case "$(uname -s)" in
  Linux*) PLATFORM=linux;;
  Darwin*) PLATFORM=osx;;
esac

# Number of parallel builds. Default: One less than the number of cores.
export JOBS=$( (nproc --ignore=1 || gnproc --ignore=1) 2>/dev/null )
