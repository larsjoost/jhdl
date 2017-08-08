#!/bin/bash

FILENAME=$1
TMPFILE="${FILENAME}~"

./version.sh > $TMPFILE

if [ ! -e $FILENAME ]; then
    cp $TMPFILE $FILENAME
else
    CHANGE=$(diff $FILENAME $TMPFILE)

    if [ -n "$CHANGE" ]; then
        cp $TMPFILE $FILENAME
    fi
fi
                                      
