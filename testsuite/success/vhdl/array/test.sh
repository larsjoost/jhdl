#!/bin/bash

source ../../../envsetup.sh

#analyse test.vhdl

simulate -d run.do $@ test.vhdl
