#!/bin/bash

DICT_DIR="$1"
DICT_NAME="$2"
DICT_VERSION="$3"
TARGET_DIR="$4"

PKG_NAME="morfeusz-dictionary-${DICT_NAME}"

TMP_DIR=`mktemp -d`
PKG_DIR="$TMP_DIR"/"$PKG_NAME"_"$DICT_VERSION"
TARGET_DICT_DIR="$PKG_DIR/usr/share/morfeusz/dictionaries"
mkdir -p "$TARGET_DICT_DIR"
cp "$DICT_DIR/$DICT_NAME-a.dict" "$TARGET_DICT_DIR"
cp "$DICT_DIR/$DICT_NAME-s.dict" "$TARGET_DICT_DIR"
mkdir "$PKG_DIR/DEBIAN"

CONTROL_FILE="$PKG_DIR/DEBIAN/control"

cat >"$CONTROL_FILE" <<EOL
Package: morfeusz-dictionary-${DICT_NAME}
Version: ${DICT_VERSION}
Section: devel
Priority: optional
Architecture: all
Maintainer: Michał Lenart <michal.lenart@ipipan.waw.pl>
Depends: morfeusz2
Description: ${DICT_NAME} dictionary for Morfeusz.
EOL

chmod 0644 "$CONTROL_FILE"

fakeroot dpkg-deb --build "$PKG_DIR" "$TARGET_DIR"
