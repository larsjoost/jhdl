#!/bin/bash

WORKDIR=$(pwd)

TEST_SCRIPT_NAME=test.sh

RESULT=0

TEST=("success" "failure")

red=1
green=2

function echo_color {
    local size
    let "size=$1 + 10"
    local c=$3
    local color=$(tput setaf $3)
    local reset=$(tput sgr0)
    printf "%-${size}s${color}$4${reset} $5\n" "$2"
}

function max {
    echo "$1" | sed 's/ /\n/g' | awk '{print length}' | sort -nr | head
}

for t in ${TEST[@]}; do

    cd $WORKDIR
    
    echo "Test case expect result = $t"

    TEST_RUNS=$(find "$t/" -name $TEST_SCRIPT_NAME)

    MAX_SIZE=$(max $TEST_RUNS)

    if [ "$t" == "success" ]; then
	EXPECTED_EXIT_VALUE=0
    else
	EXPECTED_EXIT_VALUE=1
    fi
    
    for test_run in $TEST_RUNS; do
	DIR=${test_run%/*}
	cd $WORKDIR/$DIR
	ERROR_MESSAGE=$(./${TEST_SCRIPT_NAME} 2>&1 > /dev/null)
	EXIT_VALUE=$?
        MESSAGE=""
	if [ "$t" == "success" ]; then
	    if [ -n "$ERROR_MESSAGE" ]; then
	       MESSAGE="(Unexpected error message)"
	       RESULT=1
	    fi
	else
	    if [ -z "$ERROR_MESSAGE" ]; then
	       MESSAGE="(No error message)"
	       RESULT=1
	    fi

	fi
        if [ $EXIT_VALUE -eq $EXPECTED_EXIT_VALUE ] && [ -z "$MESSAGE" ] ; then
            echo_color $MAX_SIZE "$test_run" $green "[SUCCESS]" 
	else
            echo_color $MAX_SIZE "$test_run" $red "[FAILURE]" "$MESSAGE"
            RESULT=1
	fi
    done
done
    
exit $RESULT
