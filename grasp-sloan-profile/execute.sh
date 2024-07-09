#!/bin/bash

# Specify the directory containing the .mtx files (replace with your path)
dir="everstine"

# Specify the output filename (replace with your desired name)
output_file="output-everstine.txt"

# Loop through all files ending with .mtx
#for file in "$dir"/*.mtx; do
#  ./grasp-sloan "$file" >> "$output_file" #2>&1
#done

output_file="output-sloan.txt"

# Loop through all files ending with .mtx
#for file in "$dir"/*.mtx; do
#  ./sloan "$file" >> "$output_file" #2>&1
#done

output_file="output-grasp.txt"

# Loop through all files ending with .mtx
for file in "$dir"/*.mtx; do
  # Capture the command output and redirect it to the output file
  ./grasp "$file" >> "$output_file" #2>&1
done
