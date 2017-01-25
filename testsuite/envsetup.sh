#!/bin/bash

set -e

SCRIPTPATH=$(dirname ${BASH_SOURCE[0]})

if [ -z "$JHDL" ]; then
    JHDL=$SCRIPTPATH/../src
fi

if [ -n "$VALGRIND" ]; then
    JHDL="valgrind --leak-check=yes $JHDL"
else
    if [ -n "$DEBUG" ]; then
        JHDL="gdb --args $JHDL"
    fi

    if [ -n "$VERBOSE" ]; then
        JHDL="$JHDL -v"
    fi
fi
    
function analyse {
    $JHDL/jhdl -f $1
}

function simulate {
    $JHDL/hdls $1
}
