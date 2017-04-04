#!/bin/bash

set -e

SCRIPTPATH=$(dirname ${BASH_SOURCE[0]})

export JHDL=$(realpath $SCRIPTPATH/..)

$JHDL/src/hdlc -c $JHDL/config/jhdl.conf $@
