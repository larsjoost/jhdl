#!/bin/bash

files=$(ls unit_*py)

EXIT_CODE=0

for i in $files; do
    echo "$i"
    python -m unittest $i
    let "EXIT_CODE|=$?"
done

exit $EXIT_CODE
