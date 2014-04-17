#!/bin/bash

python morfeusz_builder --input-files=../input/PoliMorfSmall.tab,../input/dodatki.tab --tagset-file=../input/sgjp-morfeusz.tagset --segments-file=../input/segmenty.dat --analyzer --serialization-method=V2 --trim-supneg -o $1
