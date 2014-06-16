#!/bin/bash

file=$1

for encoding in ISO8859-2 CP1250 CP852
do
    iconv -f UTF-8 -t $encoding < $file > $file.$encoding
done
