#!/bin/bash

set -exo pipefail

DIR=$1
shift
ARGS=`cat $DIR/ARGS`

DICT_FILE=`mktemp`

python fsabuilder/morfeusz_builder \
    --analyzer \
    --input-files $DIR/dictionary.tab \
    -o $DICT_FILE \
    --tagset-file=$DIR/tagset.dat \
    --segments-file=$DIR/segmentation.dat \
    --serialization-method=V1

build/morfeusz/morfeusz_analyzer -i $DICT_FILE $ARGS < $DIR/input.txt > $DIR/output.txt
