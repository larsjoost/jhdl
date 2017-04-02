#!/bin/bash

set -e

SCRIPTPATH=$(dirname ${BASH_SOURCE[0]})

$SCRIPTPATH/hdlc -s $SCRIPTPATH/../std/standard.vhd $@
