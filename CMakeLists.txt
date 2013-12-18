
cmake_minimum_required (VERSION 2.8) 
project (Morfeusz) 
set (Morfeusz_VERSION_MAJOR 0)
set (Morfeusz_VERSION_MINOR 1)
set (CMAKE_BUILD_TYPE "Release")

set (CMAKE_CXX_FLAGS "-std=c++98 -Wall -pedantic -Wcast-align -Wextra -Wmissing-noreturn -Wconversion -Wcast-qual -Wcast-align")
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")

enable_testing()

file (COPY fsabuilder testfiles pymorfeusz DESTINATION .)

configure_file (
  "${PROJECT_SOURCE_DIR}/morfeusz/MorfeuszConfig.hpp.in"
  "${PROJECT_BINARY_DIR}/morfeusz/MorfeuszConfig.hpp"
)

include_directories("${PROJECT_BINARY_DIR}" )

add_subdirectory (morfeusz)

set (PYMORFEUSZ _morfeusz)
set (JMORFEUSZ jmorfeusz)

###### add some dependencies for SWIG modules ######
add_dependencies (${PYMORFEUSZ} morfeusz)
add_dependencies (${JMORFEUSZ} morfeusz)

########## JAVA ##########

# copy jmorfeusz to java source tree
add_custom_target (copy-jmorfeusz-libs ALL
          COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_SHARED_LIBRARY_PREFIX}jmorfeusz${CMAKE_SHARED_LIBRARY_SUFFIX} ${PROJECT_SOURCE_DIR}/jmorfeusz/src/main/native
          COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_SHARED_LIBRARY_PREFIX}morfeusz${CMAKE_SHARED_LIBRARY_SUFFIX} ${PROJECT_SOURCE_DIR}/jmorfeusz/src/main/native
          DEPENDS ${JMORFEUSZ})

# build jmorfeusz
# add_custom_command(TARGET all POST_BUILD mvn package -PotherOutputDir -DoutputDir=${PROJECT_BINARY_DIR}/jmorfeusz/target WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/jmorfeusz)
add_custom_target ( jmorfeusz-maven-package ALL
    COMMAND mvn package -PotherOutputDir -DoutputDir=${PROJECT_BINARY_DIR}/jmorfeusz
    DEPENDS copy-jmorfeusz-libs
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/jmorfeusz)

########## PYTHON ##########

# copy python->c++ bindings
add_custom_target (copy-pymorfeusz-libs ALL
          COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${PYMORFEUSZ}${CMAKE_SHARED_LIBRARY_SUFFIX} ${PROJECT_BINARY_DIR}/pymorfeusz
          DEPENDS ${PYMORFEUSZ})
add_custom_target (copy-pymorfeusz-wrapper ALL
          COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_BINARY_DIR}/morfeusz/python/morfeusz.py ${PROJECT_BINARY_DIR}/pymorfeusz
          DEPENDS ${PYMORFEUSZ})

add_test (TestPyMorfeusz python pymorfeusz/test.py)

########## tests ##########

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
test_result_equals (testfiles/test1.txt.CP852     testfiles/test1.txt.out.CP852     CP852)
