#!/bin/bash

set -e

SCRIPTPATH=$(dirname ${BASH_SOURCE[0]})

OPTIONS=""

if [ -z "$JHDL" ]; then
    JHDL=$SCRIPTPATH/../src
fi

if [ -n "$VERBOSE" ]; then
    VERBOSE="-v"
fi

function analyse {
    $JHDL/hdlc.sh -f $1 $VERBOSE
}

function simulate {
    $JHDL/hdls -f $@ $HDLS_DEBUG
}
