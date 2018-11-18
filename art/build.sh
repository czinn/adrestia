#!/bin/bash
# Renders our SVG assets to PNGs.

SVG_DIR="assets/"
OUTPUT_DIR="build/"
DPI=300

find $SVG_DIR -type f -name '*.svg' -print0 | while IFS= read -r -d $'\0' input_file; do
  # assets/xxx.svg -> build/xxx.png
  base_file="${input_file#$SVG_DIR}"
  output_file="$OUTPUT_DIR/${base_file%.svg}.png"
  mkdir -p $(dirname $output_file)

  # Render iff output is newer than input
  if [[ ! -e $output_file ]] || [[ "$input_file" -nt $output_file ]]; then
    echo "Rendering $base_file"
    inkscape \
      --export-png="$output_file" \
      --export-dpi=$DPI \
      --export-background-opacity=0 \
      --without-gui \
      "$input_file"
  else
    echo "Skipping $base_file"
  fi
done
