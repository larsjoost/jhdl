#!/bin/bash

set -e

SCRIPTPATH=$(dirname ${BASH_SOURCE[0]})

if [ -z "$JHDL" ]; then
    JHDL=$SCRIPTPATH/../src/jhdl
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
    $JHDL -f $1
}

function simulate {
    local filename=$1
    local filebase="${filename%.*}"
    local cppfile="$filebase.cpp"
    $JHDL -f $filename > $cppfile
    g++ -std=c++11 -g  -I$SCRIPTPATH/../src/kernel/lib -o $filebase $cppfile
    ./$filebase
}
