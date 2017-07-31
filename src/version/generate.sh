#!/bin/bash

FILENAME=$1
TMPFILE="${FILENAME}~"

./version.sh > $TMPFILE

CHANGE=$(diff $FILENAME $TMPFILE)

if [ -n "$CHANGE" ]; then
    cp $TMPFILE $FILENAME
fi
                                      
