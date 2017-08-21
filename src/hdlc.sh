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

which astyle > /dev/null
EXIT_CODE=$?
if [ $EXIT_CODE -eq 0 ]; then
    echo "Warning: Call of astyle is not implemented yet"
else
    echo "Warning: astyle is not installed (dnf install astyle). Code will not be beautified"
fi
