#!/bin/bash

python buildfsa.py --input-files=../input/PoliMorfSmall.tab,../input/dodatki.tab \
    --tagset-file=/tmp/polimorf-sgjp.tagset \
    --segments-file=/tmp/segmenty.dat \
    --analyzer \
    --serialization-method=SIMPLE \
    --trim-supneg \
    -o $1
