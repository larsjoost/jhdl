#!/bin/bash

set -e

SCRIPT=$(realpath $0)
SCRIPTPATH=$(dirname $SCRIPT)

function error {
    local message=$1
    echo "[ERROR] $SCRIPT: $message" 1>&2
}

export JHDL=$SCRIPTPATH/..

source $SCRIPTPATH/setup.sh

if [ -n "$DEBUG" ]; then
    OPTIONS="gdb --args"
    HDLS_DEBUG="-x"
fi

if [ -n "$VALGRIND" ]; then
    OPTIONS="valgrind --leak-check=yes"
fi

$OPTIONS $JHDL/src/hdlc -c $JHDL/config/jhdl.conf $@

