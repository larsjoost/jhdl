#!/bin/bash

SCRIPTPATH=$(dirname ${BASH_SOURCE[0]})

function error {
    local message=$1
    echo "[ERROR] $SCRIPT: $message" 1>&2
}

if [ -z "$JHDL" ]; then
    export JHDL=$SCRIPTPATH
fi

if [ -z "$SYSTEMC" ]; then
    SYSTEMC=/opt/systemc
fi

BIN_PATH=$(realpath $SCRIPTPATH/bin)

BIN_IN_PATH=$(echo $PATH | grep $BIN_PATH)

if [ -z "$BIN_IN_PATH" ]; then
    export PATH=$PATH:$BIN_PATH
fi

