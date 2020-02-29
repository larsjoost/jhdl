#!/bin/bash

source ../../../envsetup.sh

# analyse test.vhdl

#compile 

simulate test.vhdl -d run.do $@

