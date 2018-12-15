#!/bin/bash
OUTPUT="rules.json"
if type mypy > /dev/null; then
	mypy --check-untyped-defs --no-implicit-optional rules.gen.py || exit 1
else
	echo 'mypy not found, so could not typecheck'
fi
python rules.gen.py > $OUTPUT
if type ajv > /dev/null; then
	ajv -s rules_schema.json -d $OUTPUT
else
	echo 'ajv not found, so could not verify output against json schema'
fi
