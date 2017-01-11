#!/bin/bash

WORKDIR=$(pwd)

TEST_SCRIPT_NAME=test.sh

RESULT=0

TEST=("success" "failure")

red=1
green=2

function echo_color {
    color=$(tput setaf $1)
    reset=$(tput sgr0)
    echo "$(tput setaf $1)$2$(tput sgr0)"
}

for t in ${TEST[@]}; do

    cd $WORKDIR
    
    echo "Test case expect result = $t"

    TEST_RUNS=$(find "$t/" -name $TEST_SCRIPT_NAME)

    if [ "$t" == "success" ]; then
	EXPECTED_EXIT_VALUE=0
    else
	EXPECTED_EXIT_VALUE=1
    fi
    
    for i in $TEST_RUNS; do
	DIR=${i%/*}
	cd $WORKDIR/$DIR
	ERROR_MESSAGE=$(./${TEST_SCRIPT_NAME} 2>&1 > /dev/null)
	EXIT_VALUE=$?
	echo -n "$i "
	if [ "$t" == "success" ]; then
	    if [ -n "$ERROR_MESSAGE" ]; then
	       echo -n "Unexpected error message, "
	       RESULT=1
	    fi
	else
	    if [ -z "$ERROR_MESSAGE" ]; then
	       echo -n "No error message, "
	       RESULT=1
	    fi

	fi
	if [ $EXIT_VALUE -eq $EXPECTED_EXIT_VALUE ]; then
            echo_color "[SUCCESS]" $red
	else
            echo_color "[FAILURE]" $green
            RESULT=1
	fi
    done
done
    
exit $RESULT
