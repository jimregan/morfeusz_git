#!/bin/bash

set -e -o pipefail

if [ "$#" -ne 4 ]; then
    echo "Must provide exactly 4 arguments: <CROSSMORFEUSZ_ROOT> <INPUT_DICTIONARIES> <DEFAULT_DICT_NAME> <DICT_VERSION>"
    exit 1
fi

export CROSSMORFEUSZ_ROOT="$1"
export INPUT_DICTIONARIES="$2"
export DEFAULT_DICT_NAME="$3"
export DICT_VERSION="$4"
export ANALYZER_DICTIONARY_CPP=`mktemp`.cpp
export GENERATOR_DICTIONARY_CPP=`mktemp`.cpp
export DICT_DIR=`mktemp -d`

function buildDictionaries {
    
    INPUT_TAGSET=input/morfeusz-sgjp.tagset
    SEGMENT_RULES_FILE=input/segmenty.dat
    
    python fsabuilder/morfeusz_builder \
        --input-files="$INPUT_DICTIONARIES" \
        --tagset-file="$INPUT_TAGSET" \
        --segments-file="$SEGMENT_RULES_FILE" \
        --analyzer-cpp="$ANALYZER_DICTIONARY_CPP" \
        --generator-cpp="$GENERATOR_DICTIONARY_CPP" \
        --dict="$DEFAULT_DICT_NAME" \
        --dict-dir="$DICT_DIR"
    
    echo "DONE building dictionaries" >&2
}

function build {
    set -e -o pipefail
    os=$1
    arch=$2
    embedded=$3
    python_ver=$4
    shift
    shift
    shift
    shift
    targets=$@
    
    srcDir=`pwd`
    buildDir=buildall/$os-$arch-$embedded
    targetDir=$srcDir/target
    toolchain=$srcDir/toolchains/Toolchain-$os-$arch.cmake
    
    echo "Will use $toolchain toolchain"

    # rm -rf $buildDir
    #~ rm -rf $targetDir
    rm -f $buildDir/CMakeCache.txt
    mkdir -p $buildDir
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
            -D DEFAULT_DICT_NAME=$DEFAULT_DICT_NAME \
            -D SKIP_DICTIONARY_BUILDING=1 \
            -D EMBEDDED_DEFAULT_DICT=1 \
            -D DICT_VERSION=$DICT_VERSION"
        if [ "$CPACK_GENERATOR" != "" ]
        then
            CMAKE_ARGS="$CMAKE_ARGS -D CPACK_GENERATOR=$CPACK_GENERATOR"
        fi
    else
        echo "setting default ACL to prevent control-file-has-bad-permissions lintian error"
        setfacl -R -d -m o::rx -m g::rx -m u::rwx .
        
        CMAKE_ARGS="-D CROSSMORFEUSZ_ROOT=$CROSSMORFEUSZ_ROOT \
            -D CMAKE_TOOLCHAIN_FILE=$toolchain \
            -D TARGET_DIR=$targetDir \
            -D DEFAULT_DICT_DIR=$DICT_DIR \
            -D DEFAULT_DICT_NAME=$DEFAULT_DICT_NAME \
            -D SKIP_DICTIONARY_BUILDING=1 \
            -D CPACK_GENERATOR=DEB \
            -D DICT_VERSION=$DICT_VERSION"
    fi
    CMAKE_ARGS="$CMAKE_ARGS -D PY=$python_ver"
    cmake $CMAKE_ARGS $srcDir 2>&1
    echo "building $toolchain" >&2
    make
    make $targets

    for f in `find "$targetDir" -name "*-Linux-*.deb"`
    do 
        mv "$f" "`echo $f | sed -r 's/Linux-amd64.deb$/amd64.deb/' | sed -r 's/Linux-i386.deb/i386.deb/'`"
    done

    for f in `find "$targetDir" -name "*-Linux-*.egg"`
    do 
        mv "$f" "`echo $f | sed -r 's/Linux-amd64.egg$/linux-x86_64.egg/' | sed -r 's/Linux-i386.egg$/linux-i686.egg/'`"
    done

    if [ "$os" = Linux -a "$embedded" = false ]; then
        "$srcDir/createDictionaryDeb.sh" "$DICT_DIR" "${DEFAULT_DICT_NAME}" "${DICT_VERSION}" "${targetDir}"
    fi

    cd "$srcDir"
}

function log {
    os=$1
    arch=$2
    stdbuf -oL sed -e $"s/^/$os-$arch:\t/" | tee "log/$os-$arch.out"
}

function buildegg {
    os=$1
    arch=$2
    embedded=$3
    python_ver=$4

    srcDir=`pwd`
    buildDir=buildall/$os-$arch-$embedded/morfeusz/wrappers/python$python_ver
    if [ "$python_ver" = 2 ]
    then
        eggName=morfeusz2-0.4.0-py2.7
        eggDir=$buildDir/$eggName
        pythonIncl=python27
        if [ "$os-$arch" == "Windows-amd64" ]
        then
            pythonDir=$CROSSMORFEUSZ_ROOT/windows64/Python27
        elif [ "$os-$arch" == "Windows-i386" ]
        then
            pythonDir=$CROSSMORFEUSZ_ROOT/windows32/Python27
        elif [ "$os-$arch" == "Linux-i386" ]
        then
            pythonDir=$CROSSMORFEUSZ_ROOT/linux32/python/include/python2.7
        elif [ "$os-$arch" == "Darwin-amd64" ]
        then
            pythonIncl=python2.7
            pythonDir=$CROSSMORFEUSZ_ROOT/darwin64/Python.framework/Headers
            pythonLib=$CROSSMORFEUSZ_ROOT/darwin64/Python.framework/Versions/2.7/lib
        fi
    else
        eggName=morfeusz2-0.4.0-py3.6
        eggDir=$buildDir/$eggName
        pythonIncl=python36
        if [ "$os-$arch" == "Windows-amd64" ]
        then
            pythonDir=$CROSSMORFEUSZ_ROOT/windows64/Python36
        elif [ "$os-$arch" == "Windows-i386" ]
        then
            pythonDir=$CROSSMORFEUSZ_ROOT/windows32/Python36-32
        elif [ "$os-$arch" == "Linux-i386" ]
        then
            pythonDir=$CROSSMORFEUSZ_ROOT/linux32/python3/include/python3.4m
        elif [ "$os-$arch" == "Darwin-amd64" ]
        then
            pythonIncl=python3.6
            pythonDir=$CROSSMORFEUSZ_ROOT/darwin64/Python3.framework/Versions/3.6/Headers
            pythonLib=$CROSSMORFEUSZ_ROOT/darwin64/Python3.framework/Versions/3.6/lib
        fi
    fi

    targetDir=$srcDir/target
    echo "$srcDir"
    echo "$buildDir"
    echo "$eggDir"
    echo "$pythonDir"
    if [ "$os-$arch" == "Windows-amd64" ]
    then
        gcc_command="x86_64-w64-mingw32-gcc -pthread -static-libgcc -static-libstdc++ -std=c++98 -DNDEBUG -DMS_WIN64 -g -fwrapv -O2 -Wall -Wstrict-prototypes -fno-strict-aliasing -Wdate-time -D_FORTIFY_SOURCE=2 -g -fno-stack-protector -Wformat -Werror=format-security -fPIC -I$srcDir/morfeusz -I$buildDir/../../.. -I$pythonDir/include -c $buildDir/swigPYTHON.cpp -o $buildDir/swigPYTHON.o"
        echo "$gcc_command"
        eval $gcc_command
        gpp_command="x86_64-w64-mingw32-g++ -pthread -static-libgcc -static-libstdc++ -DMS_WIN64 -shared -Wl,-O1 -Wl,-Bsymbolic-functions -Wl,-Bsymbolic-functions -fno-strict-aliasing -DNDEBUG -g -fwrapv -O2 -Wall -Wstrict-prototypes -Wdate-time -D_FORTIFY_SOURCE=2 -g -fno-stack-protector -Wformat -Werror=format-security -Wl,-Bsymbolic-functions -Wdate-time -D_FORTIFY_SOURCE=2 -g -fno-stack-protector -Wformat -Werror=format-security $buildDir/swigPYTHON.o -L$buildDir/../.. -L$pythonDir/libs -lmorfeusz2 -l$pythonIncl -o $eggDir/_morfeusz2.pyd"
        echo "$gpp_command"
        eval $gpp_command
    elif [ "$os-$arch" == "Windows-i386" ]
    then
        gcc_command="i686-w64-mingw32-gcc -pthread -static-libgcc -static-libstdc++ -std=c++98 -DNDEBUG -DMS_WIN64 -g -fwrapv -O2 -Wall -Wstrict-prototypes -fno-strict-aliasing -Wdate-time -D_FORTIFY_SOURCE=2 -g -fno-stack-protector -Wformat -Werror=format-security -fPIC -I$srcDir/morfeusz -I$buildDir/../../.. -I$pythonDir/include -c $buildDir/swigPYTHON.cpp -o $buildDir/swigPYTHON.o"
        echo "$gcc_command"
        eval $gcc_command
        gpp_command="i686-w64-mingw32-g++ -pthread -static-libgcc -static-libstdc++ -DMS_WIN64 -shared -Wl,-O1 -Wl,-Bsymbolic-functions -Wl,-Bsymbolic-functions -fno-strict-aliasing -DNDEBUG -g -fwrapv -O2 -Wall -Wstrict-prototypes -Wdate-time -D_FORTIFY_SOURCE=2 -g -fno-stack-protector -Wformat -Werror=format-security -Wl,-Bsymbolic-functions -Wdate-time -D_FORTIFY_SOURCE=2 -g -fno-stack-protector -Wformat -Werror=format-security $buildDir/swigPYTHON.o -L$buildDir/../.. -L$pythonDir/libs -lmorfeusz2 -l$pythonIncl -o $eggDir/_morfeusz2.pyd"
        echo "$gpp_command"
        eval $gpp_command
    elif [ "$os-$arch" == "Linux-i386" ]
    then
        gcc_command="x86_64-linux-gnu-gcc -m32 -pthread -DNDEBUG -g -fwrapv -O2 -Wall -Wstrict-prototypes -g -O2 -fstack-protector-strong -Wformat -Werror=format-security -Wdate-time -D_FORTIFY_SOURCE=2 -fPIC -I$srcDir/morfeusz -I$buildDir/../../.. -I$pythonDir -I$pythonDir/.. -c $buildDir/swigPYTHON.cpp -o $buildDir/swigPYTHON.o"
        echo "$gcc_command"
        eval $gcc_command
        gpp_command="x86_64-linux-gnu-g++ -m32 -pthread -shared -Wl,-O1 -Wl,-Bsymbolic-functions -Wl,-Bsymbolic-functions -specs=/usr/share/dpkg/no-pie-link.specs -Wl,-z,relro -Wl,-Bsymbolic-functions -Wl,-z,relro -g -O2 -fstack-protector-strong -Wformat -Werror=format-security -Wdate-time -D_FORTIFY_SOURCE=2 $buildDir/swigPYTHON.o -L$buildDir/../.. -lmorfeusz2 -o $eggDir/_morfeusz2.so"
        echo "$gpp_command"
        eval $gpp_command
    elif [ "$os-$arch" == "Darwin-amd64" ]
    then
        gcc_command="/home/zil/crossmorfeusz/darwin64/x86_64-apple-darwin9/bin/x86_64-apple-darwin9-gcc -std=c++98 -pthread -DNDEBUG -g -fwrapv -O2 -Wall -Wstrict-prototypes -g -O2 -Wformat -D_FORTIFY_SOURCE=2 -fPIC -I$srcDir/morfeusz -I$buildDir/../../.. -I$pythonDir -I$pythonDir/.. -c $buildDir/swigPYTHON.cpp -o $buildDir/swigPYTHON.o"
        echo "$gcc_command"
        eval $gcc_command
        gpp_command="/home/zil/crossmorfeusz/darwin64/x86_64-apple-darwin9/bin/x86_64-apple-darwin9-g++ -std=c++98 -Wall -O3 -DNDEBUG -dynamiclib -Wl,-headerpad_max_install_names -install_name _morfeusz2.so $buildDir/swigPYTHON.o -L$buildDir/../.. -L$pythonLib -lmorfeusz2 -l$pythonIncl -o $eggDir/_morfeusz2.so"
        echo "$gpp_command"
        eval $gpp_command
    fi


    if [ "$os" == "Windows" ]
    then
        cp $buildDir/../../morfeusz2.dll $eggDir/
    elif  [ "$os" == "Linux" ]
    then
        cp $buildDir/../../libmorfeusz2.so $eggDir/
    elif  [ "$os" == "Darwin" ]
    then
        cp $buildDir/../../libmorfeusz2.dylib $eggDir/
    fi

    cp $buildDir/morfeusz2.py $eggDir/
    cd $buildDir/$eggName
    if [ "$os-$arch" == "Windows-amd64" ]
    then
        zip -r $eggName-win-amd64.zip ./*
        cp $eggName-win-amd64.zip $targetDir/$eggName-win-amd64.egg
    elif [ "$os-$arch" == "Windows-i386" ]
    then
        zip -r $eggName-win32.zip ./*
        cp $eggName-win32.zip $targetDir/$eggName-win32.egg
    elif [ "$os-$arch" == "Linux-i386" ]
    then
        zip -r $eggName-linux-i686.zip ./*
        cp $eggName-linux-i686.zip $targetDir/$eggName-linux-i686.egg
    elif [ "$os-$arch" == "Darwin-amd64" ]
    then
        zip -r $eggName-macosx-10.9-x86_64.zip ./*
        cp $eggName-macosx-10.9-x86_64.zip $targetDir/$eggName-macosx-10.9-x86_64.egg
    fi

}

export -f build
export -f log
export -f buildegg

rm -rf log target buildall
mkdir -p log buildall

buildDictionaries 2>&1 | log All all

{
    # echo "build Linux amd64 true 2 package package-java package-python2 package-builder 2>&1 | log Linux-tgz2 amd64; \
    #     build Linux amd64 true 3 package-python3 2>&1 | log Linux-tgz3 amd64"
    # echo "build Linux amd64 false 0 lib-deb bin-deb dev-deb dictionary-deb java-deb 2>&1 | log Linux-deb amd64"
    # echo "LDFLAGS=-m32;CFLAGS=-m32;CXXFLAGS=-m32 build Linux i386 true 2 package package-java py2morfeusz 2>&1 | log Linux-tgz i386; \
    #     buildegg Linux i386 true 2 2>&1 | log Linux i386; \
    #     LDFLAGS=-m32;CFLAGS=-m32;CXXFLAGS=-m32 build Linux i386 true 3 py3morfeusz 2>&1 | log Linux-tgz i386; \
    #     buildegg Linux i386 true 3 2>&1 | log Linux i386"
    # echo "LDFLAGS=-m32;CFLAGS=-m32;CXXFLAGS=-m32 build Linux i386 false 0 lib-deb bin-deb java-deb 2>&1 | log Linux-deb i386"
    # echo "build Windows amd64 true 2 package package-java py2morfeusz 2>&1 | log Windows amd64; \
    #     buildegg Windows amd64 true 2 2>&1 | log Windows amd64; \
    #     build Windows amd64 true 3 py3morfeusz 2>&1 | log Windows amd64; \
    #     buildegg Windows amd64 true 3 2>&1 | log Windows amd64"
    # echo "build Windows i386 true 2 package package-java py2morfeusz 2>&1 | log Windows i386; \
    #     buildegg Windows i386 true 2 2>&1 | log Windows i386; \
    #     build Windows i386 true 3 py3morfeusz 2>&1 | log Windows i386 \
    #     buildegg Windows i386 true 3 2>&1 | log Windows i386"
    echo "build Darwin amd64 true 2 package package-java py2morfeusz 2>&1 | log Darwin amd64; \
        buildegg Darwin amd64 true 2 2>&1 | log Darwin amd64; \
        build Darwin amd64 true 3 py3morfeusz 2>&1 | log Darwin amd64; \
        buildegg Darwin amd64 true 3 2>&1 | log Darwin amd64"
    # echo "build Darwin amd64 true 2 package package-java py2morfeusz 2>&1 | log Darwin amd64"
    # echo "buildegg Darwin amd64 true 2 2>&1 | log Darwin amd64"

} | xargs -n1 -P4 -d$'\n' bash -c


