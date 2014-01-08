
cmake_minimum_required (VERSION 2.8) 
project (Morfeusz) 
set (Morfeusz_VERSION_MAJOR 0)
set (Morfeusz_VERSION_MINOR 1)
set (CMAKE_BUILD_TYPE "Release")

##### initialize some vars #####
set (INPUT_DICTIONARY_CPP "${CMAKE_CURRENT_BINARY_DIR}/default_fsa.cpp")
if ("${INPUT_DICTIONARY}" STREQUAL "")
   if ("${EMPTY_INPUT_DICTIONARY}" STREQUAL "TRUE")
    set (INPUT_DICTIONARY ${PROJECT_SOURCE_DIR}/input/empty.txt)
   else ()
    set (INPUT_DICTIONARY ${PROJECT_SOURCE_DIR}/input/polimorf.txt)
   endif ()
endif ()
if ("${INPUT_TAGSET}" STREQUAL "")
   set (INPUT_TAGSET ${PROJECT_SOURCE_DIR}/input/polimorf.tagset)
endif ()

message ("Will use ${INPUT_DICTIONARY} as default input dictionary")

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++98 -Wall -pedantic -Wcast-align -Wextra -Wmissing-noreturn -Wconversion -Wcast-qual -Wcast-align")
endif ()
# set (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set (CMAKE_SHARED_LIBRARY_PREFIX "")
    # here is some magic - must statically link libstdc++ and libgcc
    # but exclude libgcc_eh.a as it causes "multiple definition of (...)" errors
    set (CMAKE_SHARED_LINKER_FLAGS "-s -Os -static-libstdc++ -static-libgcc -Wl,--exclude-libs,libgcc_eh.a")
    set (CMAKE_EXE_LINKER_FLAGS "-s -Os -static-libstdc++ -static-libgcc")
endif ()

enable_testing()

file (COPY fsabuilder testfiles input DESTINATION .)

configure_file (
  "${PROJECT_SOURCE_DIR}/morfeusz/MorfeuszConfig.hpp.in"
  "${PROJECT_BINARY_DIR}/morfeusz/MorfeuszConfig.hpp"
)

###### add main sources ########

include_directories("${PROJECT_BINARY_DIR}" )

add_subdirectory (morfeusz)

set (PYMORFEUSZ pymorfeusz)
set (JMORFEUSZ jmorfeusz)

###### add some dependencies for SWIG modules ######
add_dependencies (${PYMORFEUSZ} morfeusz)
add_dependencies (${JMORFEUSZ} morfeusz)

########## JAVA ##########

# find out library suffix (CMAKE_JAVA_LIBRARY_SUFFIX)
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set (CMAKE_JAVA_LIBRARY_SUFFIX ".jnilib")
else ()
    set (CMAKE_JAVA_LIBRARY_SUFFIX ${CMAKE_SHARED_LIBRARY_SUFFIX})
endif ()

# build jmorfeusz
add_custom_target ( jmorfeusz-maven-package ALL
    COMMAND mvn package -PotherOutputDir -DoutputDir=${PROJECT_BINARY_DIR}/morfeusz/java -DskipTests=true
    DEPENDS ${JMORFEUSZ}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/jmorfeusz)

########## PYTHON ##########

# find out library suffix
#~ if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    #~ set (CMAKE_PYTHON_LIBRARY_SUFFIX ".so")
#~ elseif (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    #~ set (CMAKE_PYTHON_LIBRARY_SUFFIX ".pyd")
#~ else ()
    #~ set (CMAKE_PYTHON_LIBRARY_SUFFIX ${CMAKE_SHARED_LIBRARY_SUFFIX})
#~ endif ()

# copy python->c++ bindings
#~ add_custom_target (copy-pymorfeusz-libs ALL
          #~ COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_BINARY_DIR}/morfeusz/python/pymorfeusz${CMAKE_PYTHON_LIBRARY_SUFFIX} ${PROJECT_BINARY_DIR}/pymorfeusz
          #~ DEPENDS ${PYMORFEUSZ})
#~ add_custom_target (copy-pymorfeusz-wrapper ALL
          #~ COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_BINARY_DIR}/morfeusz/python/morfeusz.py ${PROJECT_BINARY_DIR}/pymorfeusz
          #~ DEPENDS ${PYMORFEUSZ})
#~ 
#~ add_test (TestPyMorfeusz python pymorfeusz/test.py)

########## tests ##########

macro (test_build_and_recognize fname method)
    add_test (TestBuild-${method}-${fname} python fsabuilder/fsa/buildfsa.py -i testfiles/${fname} -o /tmp/test-${method}-${fname}.fsa --tagset-file=testfiles/polimorf.tagset --output-format=BINARY --serialization-method=${method})
    add_test (TestRecognize-${method}-${fname} morfeusz/test_recognize_dict /tmp/test-${method}-${fname}.fsa testfiles/${fname})
    # add_test (TestNOTRecognize-${method}-${fname} fsa/test_not_recognize /tmp/test-${method}-${fname}.fsa testfiles/out_of_dict)
    # add_test (TestSpeed-${method}-${fname} fsa/test_speed /tmp/test-${method}-${fname}.fsa testfiles/speed_test_data)
endmacro ()

macro (test_result_equals inputFilename requiredOutputFilename encoding)
    # add_test (TestBuild4ResultEquals-${dictFilename}-${requiredOutputFilename} python fsabuilder/fsa/buildfsa.py -i ${dictFilename} -o /tmp/test.fsa --tagset-file=testfiles/polimorf.tagset --output-format=BINARY --serialization-method=SIMPLE)
    add_test (TestResultEquals-${inputFilename}-${requiredOutputFilename} morfeusz/test_result_equals ${inputFilename} ${requiredOutputFilename} ${encoding})
endmacro ()

test_build_and_recognize(PoliMorfSmall.tab SIMPLE)
test_build_and_recognize(PoliMorfSmall.tab V1)
test_build_and_recognize(PoliMorfSmall.tab V2)

test_result_equals (testfiles/test1.txt             testfiles/test1.txt.out             UTF8)
test_result_equals (testfiles/test1.txt.ISO8859-2   testfiles/test1.txt.out.ISO8859-2   ISO8859_2)
test_result_equals (testfiles/test1.txt.CP1250      testfiles/test1.txt.out.CP1250      CP1250)
test_result_equals (testfiles/test1.txt.CP852     testfiles/test1.txt.out.CP852     CP852)
