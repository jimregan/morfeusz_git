#!/bin/bash

python morfeusz_builder --input-files=/mnt/storage/morfeusz/PoliMorf-0.6.7.tab,../input/dodatki.tab \
    --tagset-file=../input/polimorf.tagset \
    --segments-file=../input/segmenty.dat \
    --analyzer \
    --serialization-method=V2 \
    --trim-supneg \
    -o $1
