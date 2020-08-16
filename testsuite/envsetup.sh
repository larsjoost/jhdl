#!/bin/bash

set -x

SCRIPTPATH=$(dirname ${BASH_SOURCE[0]})

if [ -z "$JHDL" ]; then
    export JHDL=$(realpath $SCRIPTPATH/..)
fi

if [ -n "$VERBOSE" ]; then
    VERBOSE="-v"
fi

source $JHDL/setup.sh

function analyse {
    hdlc $VERBOSE $@ 
}

function compile {
    hdlg $@
    hdlm $@
}

function simulate {
    compile $@
    hdls $HDLS_DEBUG -d $SCRIPTPATH/run.do $@
}
