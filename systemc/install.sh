#!/bin/bash

JDHL=${JHDL:=$1}

set -e

SCRIPTPATH=$(dirname $(readlink -f $0))

export CXX=g++

source $SCRIPTPATH/setup.sh

cd $SYSTEMC

mkdir -p objdir

cd objdir 

../configure --enable-debug

gmake

gmake install

