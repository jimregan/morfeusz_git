#!/bin/bash

SRC_DIR="$1"
BIN_DIR="$2"
LIB_VERSION="$3"
ARCH="$4"
TARGET_DIR="$5"

PKG_NAME="morfeusz2-gui"

PKG_DIR="$BIN_DIR/CMakeFiles/gui-deb.dir/${PKG_NAME}_${LIB_VERSION}"
echo Building GUI deb in $PKG_DIR

mkdir -p "$PKG_DIR"
install -D "$SRC_DIR/gui/morfeusz.desktop" "$PKG_DIR/usr/share/applications/morfeusz.desktop"
install -D "$BIN_DIR/morfeusz.jar" "$PKG_DIR/usr/share/java/morfeusz.jar"
install -D "$SRC_DIR/gui/morfeusz.svg" "$PKG_DIR/usr/share/morfeusz2/morfeusz.svg"
install -D "$SRC_DIR/gui/morfeusz2-gui.menu" "$PKG_DIR/usr/share/menu/morfeusz2-gui"
install -D "$SRC_DIR/gui/morfeusz2-gui" "$PKG_DIR/usr/bin/morfeusz2-gui"


#SRC_DIR="$( cd "$( dirname "$0" )" && pwd )"

"$SRC_DIR/createDeb.sh" "$PKG_NAME" \
    "GUI for morphological analyzer Morfeusz 2" \
    "Graphical user inferface for the morphological analyzer and generator Morfeusz 2 (used mainly for Polish)" \
    "java-runtime, libjmorfeusz-java" "morfeusz2-gui" "$LIB_VERSION" "$ARCH" misc "$PKG_DIR" "$TARGET_DIR"
