#!/bin/bash


make clean

set -e

make
make test
