#!/bin/bash
# Kompiluje pakiet debianowy z dodatkowym słownikiem dla Morfeusza
# [Zrobione z myślą o Polimorfie, ale można użyć do innych.]
#
# Założenia:
# • kod źródłowy Morfeusza jest w MORFEUSZ_SRC
# • słowniki są już skompilowane w katalogu DICT_DIR
# • kompilacja odbędzie się w katalogu BUILD_DIR (który zostanie skasowany i utworzony)
# • wyniki zostaną umieszczone w TARGET_DIR

#set -ex -o pipefail
set -x

if [ "$#" -ne 2 ]; then
    echo "Expected arguments: ‹DICT_NAME› ‹DICT_VERSION›"
    exit 1
fi

export DICT_NAME="$1"
export DICT_VERSION="$2"

export MORFEUSZ_SRC=`pwd`/Morfeusz
export DICT_DIR=`pwd`/dicts
export CROSSMORFEUSZ_ROOT=${HOME}/crossmorfeusz
export ANALYZER_DICTIONARY_CPP=$DICT_DIR/$DICT_NAME-a.cpp
export GENERATOR_DICTIONARY_CPP=$DICT_DIR/$DICT_NAME-s.cpp

# Wersja środowiska kompilacyjnego (znaczy pod którym Ubuntu budujemy: 14.04, 16.04, 18.04, 19.04 …):
# Staje się debianowym numerem subwersji: morfeusz…_1.9.27-18.04_amd64.deb
export BUILD_ENV=18.04

export BITS=64

export BUILD_ROOT=`pwd`/build/$1
export TARGET_ROOT=`pwd`/target



function build {
    set -e -o pipefail
    os=$1
    arch=$2
    embedded=$3
    python_ver=$4
    shift 4
    targets=$@
    
#    toolchain=$MORFEUSZ_SRC/toolchains/Toolchain-$os-$arch.cmake
    toolchain=$MORFEUSZ_SRC/toolchains/Toolchain-Linux-native.cmake
    buildDir=$BUILD_ROOT/$os-$BUILD_ENV-$arch-$embedded
    targetDir=$TARGET_ROOT/$os/$BUILD_ENV/$arch
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
        CMAKE_ARGS="-D CMAKE_TOOLCHAIN_FILE=$toolchain \
            -D TARGET_DIR=$targetDir \
            -D ANALYZER_DICTIONARY_CPP=$ANALYZER_DICTIONARY_CPP \
            -D GENERATOR_DICTIONARY_CPP=$GENERATOR_DICTIONARY_CPP \
            -D DEFAULT_DICT_NAME=$DICT_NAME \
            -D SKIP_DICTIONARY_BUILDING=1 \
            -D EMBEDDED_DEFAULT_DICT=1 \
            -D DICT_VERSION=$DICT_VERSION \
	    -D BUILT_ON=$BUILD_ENV "
        if [ "$CPACK_GENERATOR" != "" ]
        then
            CMAKE_ARGS="$CMAKE_ARGS -D CPACK_GENERATOR=$CPACK_GENERATOR"
        fi
    else
        # "setting default ACL to prevent control-file-has-bad-permissions lintian error"
        setfacl -R -d -m o::rx -m g::rx -m u::rwx .
        
        CMAKE_ARGS="-D CMAKE_TOOLCHAIN_FILE=$toolchain \
            -D TARGET_DIR=$targetDir \
            -D DEFAULT_DICT_DIR=$DICT_DIR \
            -D DEFAULT_DICT_NAME=$DICT_NAME \
            -D SKIP_DICTIONARY_BUILDING=1 \
            -D CPACK_GENERATOR=DEB \
            -D DICT_VERSION=$DICT_VERSION \
	    -D BUILT_ON=$BUILD_ENV "
    fi
    CMAKE_ARGS="$CMAKE_ARGS -D PY=$python_ver"
    cmake $CMAKE_ARGS $MORFEUSZ_SRC 2>&1
    echo "building $DICT_NAME for $os-$arch ($BUILD_ENV)" >&2
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
    stdbuf -oL sed -e $"s/^/$os-$BUILD_ENV-$arch:\t/" | tee "log/$os-$BUILD_ENV-$arch-$DICT_NAME.out"
}
export -f log

##??? rm -rf log $BUILD_ROOT
mkdir -p log 

build Linux $BITS false 0 dictionary-deb 2>&1 | log Linux-deb $BITS


# {
#     echo "build Linux amd64 true 2.7 package package-java package-python2 package-builder 2>&1 | log Linux-tgz2 amd64; \
#         build Linux amd64 true 3.0 package-python3 2>&1 | log Linux-tgz3 amd64"
#     echo "build Linux amd64 false 0 lib-deb bin-deb dev-deb dictionary-deb java-deb 2>&1 | log Linux-deb amd64"
#     echo "LDFLAGS=-m32;CFLAGS=-m32;CXXFLAGS=-m32 build Linux i386 true 2.7 package package-java py2morfeusz 2>&1 | log Linux-tgz i386; \
#         buildegg Linux i386 true 2.7 2>&1 | log Linux i386; \
#         LDFLAGS=-m32;CFLAGS=-m32;CXXFLAGS=-m32 build Linux i386 true 3.0 py3morfeusz 2>&1 | log Linux-tgz i386; \
#         buildegg Linux i386 true 3.0 2>&1 | log Linux i386"
#     echo "LDFLAGS=-m32;CFLAGS=-m32;CXXFLAGS=-m32 build Linux i386 false 0 lib-deb bin-deb java-deb 2>&1 | log Linux-deb i386"
#     echo "build Windows amd64 true 2.7 package package-java py2morfeusz 2>&1 | log Windows amd64; \
#         buildegg Windows amd64 true 2.7 2>&1 | log Windows amd64; \
#         build Windows amd64 true 3.6 py3morfeusz 2>&1 | log Windows amd64; \
#         buildegg Windows amd64 true 3.6 2>&1 | log Windows amd64; \
#         build Windows amd64 true 3.7 py3morfeusz 2>&1 | log Windows amd64; \
#         buildegg Windows amd64 true 3.7 2>&1 | log Windows amd64"
#     echo "build Windows i386 true 2.7 package package-java py2morfeusz 2>&1 | log Windows i386; \
#         buildegg Windows i386 true 2.7 2>&1 | log Windows i386; \
#         build Windows i386 true 3.0 py3morfeusz 2>&1 | log Windows i386 \
#         buildegg Windows i386 true 3.0 2>&1 | log Windows i386"
#     echo "build Darwin amd64 true 2 package package-java py2morfeusz 2>&1 | log Darwin amd64; \
#         buildegg Darwin amd64 true 2 2>&1 | log Darwin amd64; \
#         build Darwin amd64 true 3 py3morfeusz 2>&1 | log Darwin amd64; \
#         buildegg Darwin amd64 true 3 2>&1 | log Darwin amd64"
#     echo "build Darwin amd64 true 2 package package-java py2morfeusz 2>&1 | log Darwin amd64"
#     echo "buildegg Darwin amd64 true 2 2>&1 | log Darwin amd64"

# } | xargs -n1 -P6 -d$'\n' bash -c
