#!/bin/bash
# Usage: ./run_pypy.sh balancer.py

echo "Activating virtualenv"
if [[ ! -d venv-pypy ]]; then
  pypy3 -m venv venv-pypy
fi
source venv-pypy/bin/activate
pip install -r requirements-pypy.txt

rm -rf untyped
mkdir -p untyped

echo "Stripping type hints"
# TODO: assumes that all python files are in the current directory
for f in *.py; do
  strip-hints $f > untyped/$f &
done
wait

echo "Running"
python untyped/$1
