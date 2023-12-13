#!/bin/sh


make


files=()

for i in *; do

    files+=("$i")

done

a=$(ls)

echo "${files[@]}"
./rn.out "${files[@]}"
