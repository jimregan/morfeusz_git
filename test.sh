#!/bin/bash

function doCheckFiles() {
    tmpOut=`mktemp`
    command=$1
    shift
    input=$1
    shift
    requiredOutput=$1
    shift
    args=$@
    $command $args < $input > $tmpOut
    diff=`diff --brief $requiredOutput $tmpOut`
    if [ "$diff" != "" ]
    then
        diff -u $requiredOutput $tmpOut >&2
        echo "#### Output for $input differs from $requiredOutput" >&2
        exit 1
    fi
}

function checkAnalyzerFiles() {
    input=$1
    shift
    requiredOutput=$1
    shift
    doCheckFiles morfeusz/morfeusz_analyzer $input $requiredOutput $@
}

checkAnalyzerFiles testfiles/test.in testfiles/test.out $@
