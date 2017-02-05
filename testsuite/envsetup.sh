#!/bin/bash

set -e

SCRIPTPATH=$(dirname ${BASH_SOURCE[0]})

OPTIONS=""

if [ -z "$JHDL" ]; then
    JHDL=$SCRIPTPATH/../src
fi

if [ -n "$VALGRIND" ]; then
    OPTIONS="valgrind --leak-check=yes"
else
    if [ -n "$DEBUG" ]; then
        OPTIONS="gdb --args"
    fi

    if [ -n "$VERBOSE" ]; then
        VERBOSE="-v"
    fi
fi
    
function analyse {
    $OPTIONS $JHDL/hdlc -f $1 $VERBOSE
}

function simulate {
    $JHDL/hdls $1
}
