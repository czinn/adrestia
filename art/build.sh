#!/bin/bash
# Renders our SVG assets to PNGs.

SVG_DIR="assets/"
OUTPUT_DIR="build/"

DPI=192
NINEPATCH_DPI=96

wd=$(pwd)

find $SVG_DIR -type f -name '*.svg' -print0 | while IFS= read -r -d $'\0' input_file; do
	input_file=$(echo "$input_file" | tr -s /)
	# assets/xxx.svg -> build/xxx.png
	base_file="${input_file#$SVG_DIR}"
	output_file=$(echo "$OUTPUT_DIR/${base_file%.svg}.png" | tr -s /)
	#echo "input file $input_file"
	#echo "output file $output_file"
	mkdir -p $(dirname $output_file)

	if [[ $input_file == ${SVG_DIR}9patch/* ]]; then
		dpi=$NINEPATCH_DPI
	else
		dpi=$DPI
	fi

	# Render iff output is newer than input, or we change this script.
	if [[ ! -e $output_file ]] || [[ "$input_file" -nt $output_file ]] || [[ "$0" -nt $output_file ]]; then
		echo "Rendering $base_file -> $output_file"
		inkscape \
			--export-png="$wd/$output_file" \
			--export-dpi=$dpi \
			--export-background-opacity=0 \
			--without-gui \
			"$wd/$input_file"
	else
		#echo "Skipping $base_file"
		true
	fi
done
