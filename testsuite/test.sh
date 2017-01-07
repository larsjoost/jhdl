#!/bin/bash

TEST_SCRIPT_NAME=test.sh

SUCCESS_TEST=$(find success/ -name $TEST_SCRIPT_NAME)

RESULT=0

for i in $SUCCESS_TEST; do
    DIR=${i%/*}
    cd $DIR
    ./${TEST_SCRIPT_NAME} > /dev/null
    EXIT_VALUE=$?
    let "RESULT |= $EXIT_VALUE"
    echo -n "$i "
    if [ $EXIT_VALUE -eq 0 ]; then
        echo "[SUCCESS]"
    else
        echo "[FAILURE]"
    fi
done
    
exit $RESULT
