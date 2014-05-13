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
    cmake -D CROSSMORFEUSZ_ROOT=/home/mlenart/opt/crossmorfeusz -D CMAKE_TOOLCHAIN_FILE=$toolchain -D TARGET_DIR=$targetDir $srcDir
    echo "building $toolchain" >&2
    make
    make $targets

    for f in `find "$targetDir" -name "*-Linux-*.deb"`
    do 
        mv "$f" "`echo $f | sed -r 's/Linux-amd64.deb$/amd64.deb/' | sed -r 's/Linux-i386.deb/i386.deb/'`"
    done

    cd "$srcDir"
}

export -f build

rm -rf log
mkdir -p log
#{
#    echo "build Linux amd64 package package-java package-python package-builder &> log/linux-amd64.out"
#    echo "LDFLAGS=-m32;CFLAGS=-m32;CXXFLAGS=-m32 build Linux i386 package package-java package-builder &> log/linux-i386.out"
#    echo "build Windows amd64 package package-java package-builder &> log/windows-amd64.out"
#    echo "build Windows i386 package package-java package-builder &> log/windows-i386.out"
#    echo "build Darwin amd64 package package-builder package-java &> log/darwin-amd64.out"
#} | xargs --null | 

parallel -j4 bash -c -- \
	"build Linux amd64 package package-java package-python package-builder &> log/linux-amd64.out" \
	"LDFLAGS=-m32;CFLAGS=-m32;CXXFLAGS=-m32 build Linux i386 package package-java package-builder &> log/linux-i386.out" \
	"build Windows amd64 package package-java package-builder &> log/windows-amd64.out" \
	"build Windows i386 package package-java package-builder &> log/windows-i386.out" \
	"build Darwin amd64 package package-builder package-java &> log/darwin-amd64.out"


