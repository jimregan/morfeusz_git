#!/bin/bash

python morfeusz_builder --input-files=../input/sgjp-hom.tab,../input/dodatki.tab \
    --tagset-file=../input/polimorf.tagset \
    --segments-file=../input/segmenty.dat \
    --generator \
    --serialization-method=SIMPLE \
    -o $1
