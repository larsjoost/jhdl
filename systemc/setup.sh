#!/bin/bash

set -e

SCRIPT=$(realpath $0)
SCRIPTPATH=$(dirname $SCRIPT)

function error {
    local message=$1
    echo "[ERROR] $SCRIPT: $message" 1>&2
}

if [ -z "$SYSTEMC" ]; then
    if [ -z "$JHDL" ]; then
	error "JHDL variable is not defined"
	exit 1
    fi
    export SYSTEMC=$(find $JHDL/systemc -name systemc-* -type d)
fi
