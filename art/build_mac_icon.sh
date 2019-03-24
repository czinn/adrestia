#!/bin/bash

wd=$(pwd)
input_file="$wd/assets/app-icon.svg"
echo $input_file

output_dir="$wd/AppIcon.iconset"
mkdir -p $output_dir

for dim in 16 32 128 256 512; do
  inkscape \
    --export-png="$output_dir/icon_${dim}x${dim}.png" \
    -w $dim \
    -h $dim \
    --without-gui \
    "$input_file" &
  inkscape \
    --export-png="$output_dir/icon_${dim}x${dim}@2x.png" \
    -w $((dim * 2)) \
    -h $((dim * 2)) \
    --without-gui \
    "$input_file" &
done
wait

iconutil -c icns AppIcon.iconset

rm -r AppIcon.iconset
