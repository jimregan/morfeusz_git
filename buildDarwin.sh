#!/bin/bash
#  Kompiluje bibliotekę i buduje paczki dystrybucyjne zawierające
#  Morfeusza i słowniki SGJP i Polimorf
#
# Założenia:
# • kod źródłowy Morfeusza jest w MORFEUSZ_SRC
# • słowniki są już skompilowane w katalogu DICT_DIR
# • kompilacja odbędzie się w katalogu BUILD_DIR (który zostanie skasowany i utworzony)
# • wyniki zostaną umieszczone w TARGET_DIR

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
export CROSSMORFEUSZ_ROOT=$HOME/crossmorfeusz
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

function buildegg {
    os=$1
    bity=$2
    embedded=$3
    python_ver=$4

    if [ "$bity" = "64" ]; then
	arch=amd64
    else
	arch=i386
    fi

    buildDir=$BUILD_ROOT/$os-$bity-$embedded/morfeusz/wrappers/python${python_ver:0:1}
    if [[ "$python_ver" =~ 2.* ]]
    then
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
        if [ "$os-$arch" == "Windows-amd64" ]
        then
            if [ "$python_ver" == "3.6" ]
            then
                pythonDir=$CROSSMORFEUSZ_ROOT/windows64/Python36
                pythonIncl=python36
            elif [ "$python_ver" == "3.7" ]
            then
                pythonDir=$CROSSMORFEUSZ_ROOT/windows64/Python37
                pythonIncl=python37
            fi
        else
            pythonIncl=python36
            if [ "$os-$arch" == "Windows-i386" ]
            then
                pythonDir=$CROSSMORFEUSZ_ROOT/windows32/Python36
            elif [ "$os-$arch" == "Linux-i386" ]
            then
                pythonDir=$CROSSMORFEUSZ_ROOT/linux32/python3/include/python3.4m
            elif [ "$os-$arch" == "Darwin-amd64" ]
            then
                pythonIncl=python3.6m
                pythonDir=$CROSSMORFEUSZ_ROOT/darwin64/anaconda3.6-macos/include/python3.6m
                pythonLib=$CROSSMORFEUSZ_ROOT/darwin64/anaconda3.6-macos/lib
                # pythonDir=$CROSSMORFEUSZ_ROOT/darwin64/Python3.framework/Versions/3.6/Headers
                # pythonLib=$CROSSMORFEUSZ_ROOT/darwin64/Python3.framework/Versions/3.6/lib
            fi
        fi
    fi

    targetDir=$TARGET_ROOT/$os/$bity
    echo 'pwd:' `pwd`

    cd $buildDir
    eggName=$(echo morfeusz2-*-py$python_ver)
    if [[ ! -d $eggName  ]]; then
	echo Egg directory $eggName not found in $buildDir!!!
	exit 1
    fi
    eggDir=$buildDir/$eggName

    echo "src_dir: $MORFEUSZ_SRC"
    echo "python_build_dir: $buildDir"
    echo "egg_name: $eggName"
    echo "egg_dir: $eggDir"
    echo "python_lib_dir: $pythonDir"
    if [ "$os-$arch" == "Windows-amd64" ]
    then
        gcc_command="x86_64-w64-mingw32-gcc -pthread -static-libgcc -static-libstdc++ -std=c++98 -DNDEBUG -DMS_WIN64 -g -fwrapv -O2 -Wall -Wstrict-prototypes -fno-strict-aliasing -Wdate-time -D_FORTIFY_SOURCE=2 -g -fno-stack-protector -Wformat -Werror=format-security -fPIC -I$MORFEUSZ_SRC/morfeusz -I$buildDir/../../.. -I$pythonDir/include -c $buildDir/swigPYTHON.cpp -o $buildDir/swigPYTHON.o"
        echo "$gcc_command"
        eval $gcc_command
        gpp_command="x86_64-w64-mingw32-g++ -pthread -static-libgcc -static-libstdc++ -DMS_WIN64 -shared -Wl,-O1 -Wl,-Bsymbolic-functions -Wl,-Bsymbolic-functions -fno-strict-aliasing -DNDEBUG -g -fwrapv -O2 -Wall -Wstrict-prototypes -Wdate-time -D_FORTIFY_SOURCE=2 -g -fno-stack-protector -Wformat -Werror=format-security -Wl,-Bsymbolic-functions -Wdate-time -D_FORTIFY_SOURCE=2 -g -fno-stack-protector -Wformat -Werror=format-security $buildDir/swigPYTHON.o -L$buildDir/../.. -L$pythonDir/libs -lmorfeusz2 -l$pythonIncl -o $eggDir/_morfeusz2.pyd"
        echo "$gpp_command"
        eval $gpp_command
    elif [ "$os-$arch" == "Windows-i386" ]
    then
        gcc_command="i686-w64-mingw32-gcc -pthread -static-libgcc -static-libstdc++ -std=c++98 -DNDEBUG -DMS_WIN64 -g -fwrapv -O2 -Wall -Wstrict-prototypes -fno-strict-aliasing -Wdate-time -D_FORTIFY_SOURCE=2 -g -fno-stack-protector -Wformat -Werror=format-security -fPIC -I$MORFEUSZ_SRC/morfeusz -I$buildDir/../../.. -I$pythonDir/include -c $buildDir/swigPYTHON.cpp -o $buildDir/swigPYTHON.o"
        echo "$gcc_command"
        eval $gcc_command
        gpp_command="i686-w64-mingw32-g++ -pthread -static-libgcc -static-libstdc++ -DMS_WIN64 -shared -Wl,-O1 -Wl,-Bsymbolic-functions -Wl,-Bsymbolic-functions -fno-strict-aliasing -DNDEBUG -g -fwrapv -O2 -Wall -Wstrict-prototypes -Wdate-time -D_FORTIFY_SOURCE=2 -g -fno-stack-protector -Wformat -Werror=format-security -Wl,-Bsymbolic-functions -Wdate-time -D_FORTIFY_SOURCE=2 -g -fno-stack-protector -Wformat -Werror=format-security $buildDir/swigPYTHON.o -L$buildDir/../.. -L$pythonDir/libs -lmorfeusz2 -l$pythonIncl -o $eggDir/_morfeusz2.pyd"
        echo "$gpp_command"
        eval $gpp_command
    elif [ "$os-$arch" == "Linux-i386" ]
    then
        gcc_command="x86_64-linux-gnu-gcc -m32 -pthread -DNDEBUG -g -fwrapv -O2 -Wall -Wstrict-prototypes -g -O2 -fstack-protector-strong -Wformat -Werror=format-security -Wdate-time -D_FORTIFY_SOURCE=2 -fPIC -I$MORFEUSZ_SRC/morfeusz -I$buildDir/../../.. -I$pythonDir -I$pythonDir/.. -c $buildDir/swigPYTHON.cpp -o $buildDir/swigPYTHON.o"
        echo "$gcc_command"
        eval $gcc_command
        gpp_command="x86_64-linux-gnu-g++ -m32 -pthread -shared -Wl,-O1 -Wl,-Bsymbolic-functions -Wl,-Bsymbolic-functions -specs=/usr/share/dpkg/no-pie-link.specs -Wl,-z,relro -Wl,-Bsymbolic-functions -Wl,-z,relro -g -O2 -fstack-protector-strong -Wformat -Werror=format-security -Wdate-time -D_FORTIFY_SOURCE=2 $buildDir/swigPYTHON.o -L$buildDir/../.. -lmorfeusz2 -o $eggDir/_morfeusz2.so"
        echo "$gpp_command"
        eval $gpp_command
    elif [ "$os-$arch" == "Darwin-amd64" ]
    then
        gcc_command="/usr/x86_64-apple-darwin9/bin/x86_64-apple-darwin9-gcc -std=c++98 -pthread -DNDEBUG -g -fwrapv -O2 -Wall -Wstrict-prototypes -g -O2 -Wformat -D_FORTIFY_SOURCE=2 -fPIC -I$MORFEUSZ_SRC/morfeusz -I$buildDir/../../.. -I$pythonDir -I$pythonDir/.. -c $buildDir/swigPYTHON.cpp -o $buildDir/swigPYTHON.o"
        eval $gcc_command
        gpp_command="/usr/x86_64-apple-darwin9/bin/x86_64-apple-darwin9-g++ -std=c++98 -Wall -O3 -DNDEBUG -dynamiclib -Wl,-headerpad_max_install_names -install_name _morfeusz2.so $buildDir/swigPYTHON.o -L$buildDir/../.. -L$pythonLib -lmorfeusz2 -l$pythonIncl -o $eggDir/_morfeusz2.so"
        eval $gpp_command
	INSTALL_NAME_TOOL=/usr/x86_64-apple-darwin9/bin/x86_64-apple-darwin9-install_name_tool
	${INSTALL_NAME_TOOL} -change /usr/x86_64-apple-darwin9/x86_64-apple-darwin9/lib/libstdc++.6.dylib /usr/lib/libstdc++.6.dylib $eggDir/_morfeusz2.so
	${INSTALL_NAME_TOOL} -change libmorfeusz2.dylib @loader_path/libmorfeusz2.dylib $eggDir/_morfeusz2.so
	# ${INSTALL_NAME_TOOL} -change libmorfeusz2.dylib @executable_path/../bin/libmorfeusz2.dylib $eggDir/_morfeusz2.so
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

    cp $buildDir/morfeusz2.egg-info/PKG-INFO $eggDir/EGG-INFO/
    cp $buildDir/morfeusz2.py $eggDir/
    if [ -d "$buildDir/$eggName" ]; then
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
    fi

}
export -f buildegg

function log {
    os=$1
    arch=$2
    stdbuf -oL sed -e $"s/^/$os-$arch:\t/" | tee -a "log/$os-$arch.out"
}
export -f log

##??? rm -rf log $BUILD_ROOT
mkdir -p log 

build Darwin $BITS true 2 package package-java gui-dmg 2>&1 | log Darwin $BITS; 
build Darwin $BITS true 2 package-python2-egg-info 2>&1 | log Darwin $BITS; 
buildegg Darwin $BITS true 2 2>&1 | log Darwin $BITS; 
build Darwin $BITS true 3 package-python3-egg-info 2>&1 | log Darwin $BITS; 
buildegg Darwin $BITS true 3 2>&1 | log Darwin $BITS
#build Darwin $BITS true 2 package package-java py2morfeusz 2>&1 | log Darwin $BITS
#buildegg Darwin $BITS true 2 2>&1 | log Darwin $BITS

