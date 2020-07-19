#!/bin/bash

set -e

SCRIPT=$(realpath $0)
SCRIPTPATH=$(dirname $SCRIPT)

function error {
    local message=$1
    echo "[ERROR] $SCRIPT: $message" 1>&2
}

for i in $@; do
    case $i in
        "-x")
            OPTIONS="gdb --args"
	    ;;
        "-y")
            OPTIONS="valgrind --leak-check=yes"
            ;;
	*)
	    ARGS="$ARGS $i"
            ;;
    esac
done

export JHDL=$SCRIPTPATH/..

source $SCRIPTPATH/setup.sh

$OPTIONS $JHDL/src/build/hdlc -c $JHDL/config/jhdl.conf $ARGS 

