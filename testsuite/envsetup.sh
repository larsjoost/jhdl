#!/bin/bash

set -e

SCRIPTPATH=$(dirname ${BASH_SOURCE[0]})

OPTIONS=""

if [ -z "$JHDL" ]; then
    export JHDL=$SCRIPTPATH/..
fi

if [ -n "$VERBOSE" ]; then
    VERBOSE="-v"
fi

function analyse {
    $JHDL/src/hdlc.sh -f $1 $VERBOSE $@
}

function compile {
    make
}

function simulate {
    $JHDL/src/hdls -f $@ $HDLS_DEBUG $@
}
