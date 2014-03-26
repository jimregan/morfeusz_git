#!/bin/bash

set -e -o pipefail

function build {
    os=$1
    arch=$2
    shift
    shift
    targets=$@
    
    srcDir=`pwd`
    buildDir=buildall/build-$os-$arch
    targetDir=$srcDir/target/$os-$arch
    toolchain=$srcDir/morfeusz/Toolchain-$os-$arch.cmake
    
    echo "Will use $toolchain toolchain"
    
    rm -rf $buildDir
    rm -rf $targetDir
    mkdir -p $buildDir
    mkdir -p $targetDir
    cd $buildDir
    cmake -D CMAKE_TOOLCHAIN_FILE=$toolchain -D TARGET_DIR=$targetDir $srcDir
    make -j4
    make $targets
    
    for f in `find "$targetDir" -name "*-Linux-*.deb"`
    do
        mv "$f" "`echo $f | sed -r 's/Linux-amd64.deb$/amd64.deb/' | sed -r 's/Linux-i386.deb/i386.deb/'`"
    done
    
    cd "$srcDir"
}

build Linux amd64 package package-java package-python package-builder
LDFLAGS=-m32;CFLAGS=-m32;CXXFLAGS=-m32 build Linux i386 package package-java package-builder
build Windows amd64 package package-java
build Windows i386 package package-java
build Darwin amd64 package package-builder package-java
