#!/bin/bash
echo "testing java wrapper"
PROJECT_BINARY_DIR=$1
cd morfeusz/wrappers/java/JMorfeuszTest
LD_LIBRARY_PATH="$PROJECT_BINARY_DIR/morfeusz:$PROJECT_BINARY_DIR/morfeusz/wrappers/java" ant -lib "$PROJECT_BINARY_DIR/morfeusz/wrappers/java" test
