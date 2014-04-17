#!/bin/bash

set -e -o pipefail

function build {
    set -e -o pipefail
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
    echo "building $toolchain" >&2
    make -j2
    make $targets

    for f in `find "$targetDir" -name "*-Linux-*.deb"`
    do 
        mv "$f" "`echo $f | sed -r 's/Linux-amd64.deb$/amd64.deb/' | sed -r 's/Linux-i386.deb/i386.deb/'`"
    done

    cd "$srcDir"
}

export -f build

mkdir -p log
{
    echo "build Linux amd64 package package-java package-python package-builder &> log/linux-amd64.out"
    echo "LDFLAGS=-m32;CFLAGS=-m32;CXXFLAGS=-m32 build Linux i386 package package-java package-builder &> log/linux-i386.out"
    echo "build Windows amd64 package package-java package-builder &> log/windows-amd64.out"
    echo "build Windows i386 package package-java package-builder &> log/windows-i386.out"
    echo "build Darwin amd64 package package-builder package-java &> log/darwin-amd64.out"
} | xargs --null -I{} -P8 -n1 bash -c "{}"

