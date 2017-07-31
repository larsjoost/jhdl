#!/bin/bash

set -e

make clean
make
make test
