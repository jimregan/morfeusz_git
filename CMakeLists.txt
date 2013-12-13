
cmake_minimum_required (VERSION 2.8) 
project (Morfeusz) 
set (Morfeusz_VERSION_MAJOR 0)
set (Morfeusz_VERSION_MINOR 1)
set (CMAKE_BUILD_TYPE "Release")

set (CMAKE_CXX_FLAGS "-std=c++98 -Wall -g -pedantic -Wcast-align -Wextra -Wmissing-noreturn -Wconversion -Wcast-qual -Wcast-align")

enable_testing()

configure_file (
  "${PROJECT_SOURCE_DIR}/morfeusz/MorfeuszConfig.hpp.in"
  "${PROJECT_BINARY_DIR}/morfeusz/MorfeuszConfig.hpp"
)

include_directories("${PROJECT_BINARY_DIR}")

add_subdirectory (morfeusz)

file(COPY fsabuilder testfiles DESTINATION .)

macro (test_build_and_recognize fname method)
    add_test (TestBuild-${method}-${fname} python fsabuilder/fsa/buildfsa.py -i testfiles/${fname} -o /tmp/test-${method}-${fname}.fsa --tagset-file=testfiles/polimorf.tagset --output-format=BINARY --serialization-method=${method})
    add_test (TestRecognize-${method}-${fname} morfeusz/test_recognize_dict /tmp/test-${method}-${fname}.fsa testfiles/${fname})
    # add_test (TestNOTRecognize-${method}-${fname} fsa/test_not_recognize /tmp/test-${method}-${fname}.fsa testfiles/out_of_dict)
    # add_test (TestSpeed-${method}-${fname} fsa/test_speed /tmp/test-${method}-${fname}.fsa testfiles/speed_test_data)
endmacro (test_build_and_recognize)

macro (test_result_equals inputFilename requiredOutputFilename encoding)
    # add_test (TestBuild4ResultEquals-${dictFilename}-${requiredOutputFilename} python fsabuilder/fsa/buildfsa.py -i ${dictFilename} -o /tmp/test.fsa --tagset-file=testfiles/polimorf.tagset --output-format=BINARY --serialization-method=SIMPLE)
    add_test (TestResultEquals-${inputFilename}-${requiredOutputFilename} morfeusz/test_result_equals ${inputFilename} ${requiredOutputFilename} ${encoding})
endmacro (test_result_equals)

test_build_and_recognize(PoliMorfSmall.tab SIMPLE)
test_build_and_recognize(PoliMorfSmall.tab V1)
test_build_and_recognize(PoliMorfSmall.tab V2)

test_result_equals (testfiles/test1.txt             testfiles/test1.txt.out             UTF8)
test_result_equals (testfiles/test1.txt.ISO8859-2   testfiles/test1.txt.out.ISO8859-2   ISO8859_2)
test_result_equals (testfiles/test1.txt.CP1250      testfiles/test1.txt.out.CP1250      CP1250)
# test_result_equals (testfiles/test1.txt.UTF16LE     testfiles/test1.txt.out.UTF16LE     UTF16LE)
