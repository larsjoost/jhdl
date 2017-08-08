#!/bin/bash


make clean

git clean -xf

set -e

make
make test
