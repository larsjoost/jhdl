#!/bin/bash

set -e

SCRIPTPATH=$(dirname $(readlink -f $0))

export CXX=g++

source $SCRIPTPATH/setup.sh

cd $SYSTEMC

mkdir objdir

cd objdir 

../configure

gmake

gmake install

