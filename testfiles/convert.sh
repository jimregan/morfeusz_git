#!/bin/bash

file=$1

for encoding in ISO8859-2 CP1250 UTF16LE
do
    iconv -f UTF-8 -t $encoding < $file > $file.$encoding
done
