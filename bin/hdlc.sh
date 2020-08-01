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

HDLC=$SCRIPTPATH/../src/build/hdlc

if [ ! -e $HDLC ]; then
    error "Could not find $HDLC. Please build file by running make -C $SCRIPTPATH/.."
    exit 1
fi

$OPTIONS $SCRIPTPATH/../src/build/hdlc -c $SCRIPTPATH/../config/jhdl.conf $ARGS 

