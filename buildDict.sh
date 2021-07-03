#! /bin/bash
#  Buduje postać binarną i pliki .cpp zawierające słowniki SGJP i Polimorf
#
# Założenia:
# • kod źródłowy Morfeusza jest w MORFEUSZ_SRC
# • słowniki źródłowe są w katalogu SRC_DICT_DIR i nazywają się ‹słownik›-‹data›.tab
# • wynik kompilacji zostanie umieszczony w katalogu DICT_DIR (który zostanie skasowany i utworzony)
# • kompilacja odbędzie się w katalogu BUILD_DIR (który zostanie skasowany i utworzony)


set -ex -o pipefail

if [ "$#" -ne 1 ]; then
    echo "Exactly 1 argument needed: ‹DICT_VERSION›"
    exit 1
fi

export MORFEUSZ_SRC=`pwd`/Morfeusz
export DICT_DIR=`pwd`/dicts
export SRC_DICT_DIR=$DICT_DIR
export BUILD_DIR=`pwd`/build/dicts

rm -rf $BUILD_DIR || true

mkdir -p $BUILD_DIR $DICT_DIR

cd $BUILD_DIR
cmake $MORFEUSZ_SRC


export DICT_VERSION="$1"

# argument: nazwa słownika
function buildDictionary {
    set -x
    DICT_NAME=$1

    MAIN_DICT=$SRC_DICT_DIR/$DICT_NAME-$DICT_VERSION.tab
    INPUT_DICTIONARIES="$MAIN_DICT,$MORFEUSZ_SRC/input/dodatki.tab,$MORFEUSZ_SRC/input/emoji.tab"
    INPUT_TAGSET=$MORFEUSZ_SRC/input/morfeusz-sgjp.tagset
    SEGMENT_RULES_FILE=$MORFEUSZ_SRC/input/segmenty.dat
    ANALYZER_DICTIONARY_CPP=$DICT_DIR/$DICT_NAME-a.cpp
    GENERATOR_DICTIONARY_CPP=$DICT_DIR/$DICT_NAME-s.cpp
    
    fsabuilder/morfeusz_builder \
        --input-files="$INPUT_DICTIONARIES" \
        --tagset-file="$INPUT_TAGSET" \
        --segments-file="$SEGMENT_RULES_FILE" \
        --analyzer-cpp="$ANALYZER_DICTIONARY_CPP" \
        --generator-cpp="$GENERATOR_DICTIONARY_CPP" \
        --dict="$DICT_NAME" \
        --dict-dir="$DICT_DIR"
    
    echo "DONE building dictionaries" >&2

    DICT_PACKAGE=morfeusz2-dictionary-${DICT_NAME}-${DICT_VERSION}.tgz
    (cd $DICT_DIR; tar czvf $DICT_PACKAGE $DICT_NAME-a.dict $DICT_NAME-s.dict )
    gzip "$MAIN_DICT"
}

export -f buildDictionary


{
    echo "buildDictionary sgjp"
    echo "buildDictionary polimorf"
} | xargs -n1 -P6 -d$'\n' bash -c
