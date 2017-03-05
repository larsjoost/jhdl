#!/bin/bash

source ../../../envsetup.sh

# analyse test.vhdl

simulate test.vhdl -d run.do $@

