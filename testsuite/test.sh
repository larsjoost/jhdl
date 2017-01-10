#!/bin/bash

WORKDIR=$(pwd)

TEST_SCRIPT_NAME=test.sh

RESULT=0

echo "Test cases that is expected to succeed"

SUCCESS_TEST=$(find success/ -name $TEST_SCRIPT_NAME)

for i in $SUCCESS_TEST; do
    DIR=${i%/*}
    cd $WORKDIR/$DIR
    ./${TEST_SCRIPT_NAME} > /dev/null
    EXIT_VALUE=$?
    echo -n "$i "
    if [ $EXIT_VALUE -eq 0 ]; then
        echo "[SUCCESS]"
    else
        echo "[FAILURE]"
        RESULT=1
    fi
done

cd $WORKDIR

echo "Test cases that is expected to fail"

FAILURE_TEST=$(find failure/ -name $TEST_SCRIPT_NAME)

for i in $FAILURE_TEST; do
    DIR=${i%/*}
    cd $WORKDIR/$DIR
    ./${TEST_SCRIPT_NAME} > /dev/null
    EXIT_VALUE=$?
    echo -n "$i "
    if [ $EXIT_VALUE -ne 0 ]; then
        echo "[SUCCESS]"
    else
        echo "[FAILURE]"
        RESULT=1
    fi
done

exit $RESULT
