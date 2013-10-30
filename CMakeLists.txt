
cmake_minimum_required (VERSION 2.8) 
project (Morfeusz) 

enable_testing()

add_subdirectory (fsa)
add_subdirectory (morfeusz)

file(COPY fsabuilder testfiles DESTINATION .)

add_test (TestBuildFSA python fsabuilder/fsa/buildfsa.py -i testfiles/test.txt -o testfiles/test.fsa -t SPELL --input-format=PLAIN --output-format=BINARY)
add_test (TestRecognize fsa/test_recognize testfiles/test.fsa testfiles/test.txt)
