#!/bin/bash

echo '=== C++ game logic ==='
cloc --exclude-ext=hpp --exclude-list-file=cloc_exclude.txt cpp

echo '=== Godot client ==='
cloc --exclude-ext=hpp --exclude-list-file=cloc_exclude.txt godot

echo '=== Rules ==='
cloc --exclude-ext=hpp --exclude-list-file=cloc_exclude.txt common

echo '=== GDNative bridge ==='
cloc --exclude-ext=hpp --exclude-list-file=cloc_exclude.txt cpp_gdnative/src

echo '=== Server ==='
cloc --exclude-ext=hpp --exclude-list-file=cloc_exclude.txt server
