#!/bin/bash

set -e

SCRIPTPATH=$(dirname ${BASH_SOURCE[0]})

export JHDL=$(realpath $SCRIPTPATH/..)

if [ -n "$DEBUG" ]; then
    OPTIONS="gdb --args"
    HDLS_DEBUG="-x"
fi

if [ -n "$VALGRIND" ]; then
    OPTIONS="valgrind --leak-check=yes"
fi

$OPTIONS $JHDL/src/hdlc -c $JHDL/config/jhdl.conf $@
