#!/bin/bash

set -e -o pipefail

export CROSSMORFEUSZ_ROOT="$1"
export INPUT_DICTIONARIES="$2"
export VERSION_SUFFIX="$3"
export CPP_DICTIONARIES_DIR=`mktemp -d`

function buildDictionaries {
    buildDir=`mktemp -d`
    srcDir=`pwd`
    cd $buildDir
    cmake -D INPUT_DICTIONARIES=$INPUT_DICTIONARIES \
        -D CPP_DICTIONARIES_DIR=$CPP_DICTIONARIES_DIR \
        $srcDir
    make -j2 dictionaries
    cd $srcDir
    rm -r $buildDir
}

function build {
    set -e -o pipefail
    os=$1
    arch=$2
    shift
    shift
    targets=$@
    
    srcDir=`pwd`
    buildDir=buildall/$os-$arch
    targetDir=$srcDir/target
    toolchain=$srcDir/morfeusz/Toolchain-$os-$arch.cmake
    
    echo "Will use $toolchain toolchain"

    rm -rf $buildDir
    rm -rf $targetDir
    mkdir -p $buildDir
    mkdir -p $targetDir
    cd $buildDir
    cmake -D CROSSMORFEUSZ_ROOT=$CROSSMORFEUSZ_ROOT \
        -D CMAKE_TOOLCHAIN_FILE=$toolchain \
        -D TARGET_DIR=$targetDir \
        -D INPUT_DICTIONARIES=$INPUT_DICTIONARIES \
        -D VERSION_SUFFIX=$VERSION_SUFFIX \
        -D CPP_DICTIONARIES_DIR=$CPP_DICTIONARIES_DIR \
        -D SKIP_DICTIONARY_BUILDING=1 \
        $srcDir 2>&1
    echo "building $toolchain" >&2
    make
    make $targets

    for f in `find "$targetDir" -name "*-Linux-*.deb"`
    do 
        mv "$f" "`echo $f | sed -r 's/Linux-amd64.deb$/amd64.deb/' | sed -r 's/Linux-i386.deb/i386.deb/'`"
    done

    cd "$srcDir"
}

function log {
    os=$1
    arch=$2
    stdbuf -oL sed -e $"s/^/$os-$arch:\t/" | tee "log/$os-$arch.out"
}

export -f build
export -f log

rm -rf log
mkdir -p log

buildDictionaries 2>&1 | log All all

{
    echo "build Linux amd64 package package-java package-python package-builder 2>&1 | log Linux amd64"
    echo "LDFLAGS=-m32;CFLAGS=-m32;CXXFLAGS=-m32 build Linux i386 package package-java 2>&1 | log Linux i386"
    echo "build Windows amd64 package package-java 2>&1 | log Windows amd64"
    echo "build Windows i386 package package-java 2>&1 | log Windows i386"
    echo "build Darwin amd64 package package-java 2>&1 | log Darwin amd64"
} | xargs -n1 -P8 -d$'\n' bash -c


