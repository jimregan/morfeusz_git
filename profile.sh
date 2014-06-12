#!/bin/bash

rm -rf profbuild
mkdir -p profbuild
cd profbuild
cmake -D INPUT_DICTIONARIES=../input/dodatki.tab,../input/PoliMorfSmall.tab -D CMAKE_BUILD_TYPE=Debug -D CMAKE_CXX_FLAGS="-g -O2" -D CMAKE_SHARED_LINKER_FLAGS="-lprofiler" -D CMAKE_EXE_LINKER_FLAGS="-lprofiler" ..
make
rm -f /tmp/morfeusz.prof
export LD_PRELOAD="/usr/lib/libprofiler.so"
export CPUPROFILE="/tmp/morfeusz.prof"
morfeusz/morfeusz_analyzer -i /home/wkieras/output/sgjp_analyzer.fsa < /mnt/storage/morfeusz/sents10k > /dev/null
### pprof --gv profbuild/morfeusz/morfeusz_analyzer /tmp/morfeusz.prof
