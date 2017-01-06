#!/bin/bash

set -e

if [ -z "$JHDL" ]; then
    JHDL=jhdl
fi

if [ -n "$VALGRIND" ]; then
    JHDL="valgrind --leak-check=yes $JHDL"
else
    if [ -n "$DEBUG" ]; then
        JHDL="gdbtui --args $JHDL"
    fi

    if [ -n "$VERBOSE" ]; then
        JHDL="$JHDL -v"
    fi
fi
    
function analyse {
    $JHDL -f $1
}
