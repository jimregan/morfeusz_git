#!/bin/bash
#  Kompiluje bibliotekę i buduje paczki dystrybucyjne zawierające
# Morfeusza i słowniki SGJP i Polimorf
#
# Założenia:
# • kod źródłowy Morfeusza jest w MORFEUSZ_SRC
# • słowniki są już skompilowane w katalogu DICT_DIR
# • kompilacja odbędzie się w katalogu BUILD_DIR (który zostanie skasowany i utworzony)
# • wyniki zostaną umieszczone w TARGET_DIR

#set -ex -o pipefail
set -x

if [ "$#" -ne 4 ]; then
    echo "Expected arguments: ‹DICT_NAME› ‹DICT_VERSION› ‹BUILD_ENV› ‹64/32›"
    exit 1
fi

export DICT_NAME="$1"
export DICT_VERSION="$2"

export MORFEUSZ_SRC=`pwd`/Morfeusz
export DICT_DIR=`pwd`/dicts
export ANALYZER_DICTIONARY_CPP=$DICT_DIR/$DICT_NAME-a.cpp
export GENERATOR_DICTIONARY_CPP=$DICT_DIR/$DICT_NAME-s.cpp

# Wersja środowiska kompilacyjnego (znaczy pod którym Ubuntu budujemy: 14.04, 16.04, 18.04, 19.04 …):
# Staje się debianowym numerem subwersji: morfeusz…_1.9.27-18.04_amd64.deb
export BUILD_ENV=$3

export BITS=$4

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
    if [ "$os" = "Linux" ]; then
	buildDir=$BUILD_ROOT/$os-$BUILD_ENV-$arch-$embedded
	targetDir=$TARGET_ROOT/$os/$BUILD_ENV/$arch
    else
	buildDir=$BUILD_ROOT/$os-$arch-$embedded
	targetDir=$TARGET_ROOT/$os/$arch
    fi
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
    echo "building for $os-$arch ($BUILD_ENV)" >&2
#    make
    make $targets
}
export -f build

function log {
    os=$1
    arch=$2
    stdbuf -oL sed -e $"s/^/$os-$BUILD_ENV-$arch:\t/" | tee -a "log/$os-$BUILD_ENV-$arch.out"
}
export -f log

##??? rm -rf log $BUILD_ROOT
mkdir -p log 


build Linux $BITS true 3.0 package package-java gui-tgz package-python3 package-builder 2>&1 | log Linux-tgz3 $BITS

# if [ -x /usr/bin/python2 ]
# then
#      build Linux $BITS true 2.7 package-python2 2>&1 | log Linux-tgz2 $BITS;
# fi

# # Kompilujemy dodatkowe wheele w środowiskach wirutalnych:
# (
#     cd $BUILD_ROOT/Linux-$BUILD_ENV-$BITS-true
#     if [ -d ~/env/ ]; then
# 	for pyenv in ~/env/* ; do
# 	    source $pyenv/bin/activate
# 	    make package-python3-whl
# 	    python --version
# 	    deactivate
# 	done
#     fi
# ) | log Linux-whl $BITS

# Pakiety debianowe bez wkompilowanego słownika:
#build Linux $BITS false 0 lib-deb bin-deb dev-deb dictionary-deb java-deb gui-deb 2>&1 | log Linux-deb $BITS

