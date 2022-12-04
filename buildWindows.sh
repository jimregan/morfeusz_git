#!/bin/bash
#  Kompiluje bibliotekę i buduje paczki dystrybucyjne zawierające
#  Morfeusza i słowniki SGJP i Polimorf
#
# Założenia:
# • kod źródłowy Morfeusza jest w MORFEUSZ_SRC
# • słowniki są już skompilowane w katalogu DICT_DIR
# • kompilacja odbędzie się w katalogu BUILD_DIR (który zostanie skasowany i utworzony)
# • wyniki zostaną umieszczone w podkatalogu TARGET_ROOT

#set -ex -o pipefail
set -x

if [ "$#" -ne 3 ]; then
    echo "Expected arguments: ‹DICT_NAME› ‹DICT_VERSION› ‹64/32›"
    exit 1
fi

export DICT_NAME="$1"
export DICT_VERSION="$2"
export BITS=$3


export MORFEUSZ_SRC=`pwd`/Morfeusz
export DICT_DIR=`pwd`/dicts
export CROSSMORFEUSZ_ROOT=${HOME}/crosslibs
export ANALYZER_DICTIONARY_CPP=$DICT_DIR/$DICT_NAME-a.cpp
export GENERATOR_DICTIONARY_CPP=$DICT_DIR/$DICT_NAME-s.cpp


export BUILD_ROOT=`pwd`/build/$1
export TARGET_ROOT=`pwd`/target



function build {
    set -e -o pipefail
    os=$1
    bity=$2
    embedded=$3
    python_ver=$4
    shift 4
    targets=$@

    if [ "$bity" = "64" ]; then
	arch=amd64
    else
	arch=i386
    fi

    toolchain=$MORFEUSZ_SRC/toolchains/Toolchain-$os-$arch.cmake

    buildDir=$BUILD_ROOT/$os-$bity-$embedded
    targetDir=$TARGET_ROOT/$os/$bity

#    rm -rf $buildDir || true
    mkdir -p $buildDir

    
#    echo "Will use $toolchain toolchain"

    # rm -rf $buildDir
    #~ rm -rf $targetDir
    mkdir -p $buildDir
    rm -f $buildDir/CMakeCache.txt
    mkdir -p $targetDir
    cd $buildDir
    
    if [ "$embedded" = true ]
    then
        if [ "$os" == "Linux" ]
        then
            CPACK_GENERATOR=TGZ
        fi
        CMAKE_ARGS="-D CROSSMORFEUSZ_ROOT=$CROSSMORFEUSZ_ROOT \
            -D CMAKE_TOOLCHAIN_FILE=$toolchain \
            -D TARGET_DIR=$targetDir \
            -D ANALYZER_DICTIONARY_CPP=$ANALYZER_DICTIONARY_CPP \
            -D GENERATOR_DICTIONARY_CPP=$GENERATOR_DICTIONARY_CPP \
            -D DEFAULT_DICT_NAME=$DICT_NAME \
            -D SKIP_DICTIONARY_BUILDING=1 \
            -D EMBEDDED_DEFAULT_DICT=1 \
            -D DICT_VERSION=$DICT_VERSION "
        if [ "$CPACK_GENERATOR" != "" ]
        then
            CMAKE_ARGS="$CMAKE_ARGS -D CPACK_GENERATOR=$CPACK_GENERATOR"
        fi
    else
        # "setting default ACL to prevent control-file-has-bad-permissions lintian error"
        #setfacl -R -d -m o::rx -m g::rx -m u::rwx .
        
        CMAKE_ARGS="-D CROSSMORFEUSZ_ROOT=$CROSSMORFEUSZ_ROOT \
            -D CMAKE_TOOLCHAIN_FILE=$toolchain \
            -D TARGET_DIR=$targetDir \
            -D DEFAULT_DICT_DIR=$DICT_DIR \
            -D DEFAULT_DICT_NAME=$DICT_NAME \
            -D SKIP_DICTIONARY_BUILDING=1 \
            -D CPACK_GENERATOR=DEB \
            -D DICT_VERSION=$DICT_VERSION "
    fi
    CMAKE_ARGS="$CMAKE_ARGS -D PY=$python_ver"
    cmake $CMAKE_ARGS $MORFEUSZ_SRC 2>&1
    echo "building for $os-$arch" >&2
#    make
    make $targets

    # for f in `find "$targetDir" -name "*-Linux-*.deb"`
    # do 
    #     mv "$f" "`echo $f | sed -r 's/Linux-amd64.deb$/amd64.deb/' | sed -r 's/Linux-i386.deb/i386.deb/'`"
    # done

    # for f in `find "$targetDir" -name "*-Linux-*.egg"`
    # do 
    #     mv "$f" "`echo $f | sed -r 's/Linux-amd64.egg$/linux-x86_64.egg/' | sed -r 's/Linux-i386.egg$/linux-i686.egg/'`"
    # done

}
export -f build

function log {
    os=$1
    arch=$2
    stdbuf -oL sed -e $"s/^/$os-$arch:\t/" | tee -a "log/$os-$arch.out"
}
export -f log

##??? rm -rf log $BUILD_ROOT
mkdir -p log 

build Windows $BITS true 2.7 package package-java gui-installer 2>&1 | log Windows $BITS
build Windows $BITS true 2.7 package-python2-fatwhl 2>&1 | log Windows $BITS

for py in 3.6 3.7 3.8 3.9 3.10 3.11
do
    if [ -d ${CROSSMORFEUSZ_ROOT}/windows${BITS}/Python${py//\./}/ ]; then
	echo Building package for Python ${py}
	build Windows $BITS true $py package-python3-fatwhl 2>&1 | log Windows $BITS
    fi
done
