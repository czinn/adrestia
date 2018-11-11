#!/bin/bash
set -e

source ./config.sh

scons platform=${PLATFORM} -j${JOBS}
