
cmake_minimum_required (VERSION 2.8) 
project (Morfeusz) 

enable_testing()

add_subdirectory (fsa)
add_subdirectory (morfeusz)

file(COPY fsabuilder testfiles DESTINATION .)

add_test (TestBuildFSA python fsabuilder/fsa/buildfsa.py -i testfiles/dict.txt -o testfiles/test.fsa -t SPELL --input-format=PLAIN --output-format=BINARY)
add_test (TestRecognize fsa/test_recognize testfiles/test.fsa testfiles/dict.txt)
add_test (TestNOTRecognize fsa/test_not_recognize testfiles/test.fsa testfiles/out_of_dict.txt)
