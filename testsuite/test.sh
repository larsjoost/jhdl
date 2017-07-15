#!/bin/bash

usage() { echo "Usage: $0 [-t <name>] " 1>&2; exit 1; }

while getopts ":t:" o; do
    case $o in
        t)
            TEST_NAME=$OPTARG
            ;;
        *)
            usage
            ;;
    esac
done

WORKDIR=$(pwd)

TEST_SCRIPT_NAME=test.sh

RESULT=0

TEST=("success" "failure")

red=1
green=2

function echo_color {
    local color=$(tput setaf $1)
    local reset=$(tput sgr0)
    printf "${color}$2${reset} $3\n"
}

function echo_size {
    local size
    let "size=$1 + 10"
    printf "%-${size}s" $2
}

function max {
    echo "$1" | sed 's/ /\n/g' | awk '{print length}' | sort -nr | head
}

SOURCE=(main.cpp sc_main.cpp)

for i in ${SOURCE[@]}; do
    a=$(find ./ -name $i)
    if [ -n "$a" ]; then
        rm $a
    fi
done

for t in ${TEST[@]}; do

    cd $WORKDIR
    
    echo "Test case expect result = $t"

    TEST_RUNS=$(find "$t/" -name $TEST_SCRIPT_NAME)

    if [ -n "$TEST_NAME" ]; then
	TEST_RUNS=$(echo $TEST_RUNS | tr ' ' '\n' | grep $TEST_NAME)
    fi
    
    MAX_SIZE=$(max $TEST_RUNS)

    if [ "$t" == "success" ]; then
	EXPECTED_EXIT_VALUE=0
    else
	EXPECTED_EXIT_VALUE=1
    fi
    
    for test_run in $TEST_RUNS; do
        echo_size $MAX_SIZE $test_run
	DIR=${test_run%/*}
	cd $WORKDIR/$DIR
	ERROR_MESSAGE=$(./${TEST_SCRIPT_NAME} 2>&1 > /dev/null)
	EXIT_VALUE=$?
        MESSAGE=""
	if [ "$t" == "success" ]; then
	    if [ -n "$ERROR_MESSAGE" ]; then
	       MESSAGE="(Unexpected error message: $ERROR_MESSAGE)"
	       RESULT=1
	    fi
	else
	    if [ -z "$ERROR_MESSAGE" ]; then
	       MESSAGE="(No error message)"
	       RESULT=1
	    fi

	fi
        if [ $EXIT_VALUE -eq $EXPECTED_EXIT_VALUE ] && [ -z "$MESSAGE" ] ; then
            echo_color $green "[SUCCESS]" 
	else
            echo_color $red "[FAILURE]" "$MESSAGE"
            RESULT=1
	fi
    done
done
    
exit $RESULT
