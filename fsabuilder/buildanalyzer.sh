#! /bin/bash

python3 morfeusz_builder --input-files=../input/sgjp-hom.tab,../input/dodatki.tab --tagset-file=../input/sgjp-morfeusz.tagset --segments-file=../input/segmenty.dat --analyzer --serialization-method=V1 --trim-supneg -o $1
