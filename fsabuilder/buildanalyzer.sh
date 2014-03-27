#!/bin/bash

python buildfsa.py --input-files=../input/PoliMorfSmall.tab,../input/dodatki.tab \
    --tagset-file=../input/polimorf.tagset \
    --segments-file=../input/segmenty.dat \
    --analyzer \
    --serialization-method=SIMPLE \
    --trim-supneg \
    -o $1