#!/bin/bash
echo "testing java wrapper"
PROJECT_BINARY_DIR=$1
LD_LIBRARY_PATH="$PROJECT_BINARY_DIR/morfeusz:$PROJECT_BINARY_DIR/morfeusz/wrappers/java" java -jar "$PROJECT_BINARY_DIR/morfeusz/wrappers/java/jmorfeusz.jar"
