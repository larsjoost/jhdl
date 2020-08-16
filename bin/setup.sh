#!/bin/bash

SCRIPTPATH=$(dirname ${BASH_SOURCE[0]})

function error {
    local message=$1
    echo "[ERROR] $SCRIPT: $message" 1>&2
}

if [ -z "$JHDL" ]; then
    error "JHDL variable is not defined"
    exit 1
fi

source $JHDL/setup.sh
