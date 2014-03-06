
cmake_minimum_required (VERSION 2.8) 
project (Morfeusz)

set (Morfeusz_VERSION_MAJOR 2)
set (Morfeusz_VERSION_MINOR 0)
set (Morfeusz_VERSION_PATCH 0)
set (CMAKE_BUILD_TYPE "Debug")

enable_testing()

##### initialize some vars #####

# ARCHITECTURE
if ("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")
    set (ARCHITECTURE "amd64")
elseif ("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86")
    set (ARCHITECTURE "i386")
elseif ("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "")
    message (FATAL_ERROR "CMAKE_SYSTEM_PROCESSOR is not set (should be x86 or x86_64)")
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set (ARCHITECTURE "amd64")
else ()
    message (FATAL_ERROR "Invalid architecture: ${CMAKE_SYSTEM_PROCESSOR}")
endif ()

# PROJECT_VERSION
set (PROJECT_VERSION "${Morfeusz_VERSION_MAJOR}.${Morfeusz_VERSION_MINOR}.${Morfeusz_VERSION_PATCH}")

### USER DEFINED VARIABLES

# INPUT_DICTIONARY_CPP
set (INPUT_DICTIONARY_CPP "${PROJECT_SOURCE_DIR}/default_fsa.cpp")
set (INPUT_SYNTH_DICTIONARY_CPP "${PROJECT_SOURCE_DIR}/default_synth_fsa.cpp")
if ("${INPUT_DICTIONARIES}" STREQUAL "")
   if ("${EMPTY_INPUT_DICTIONARY}" STREQUAL "TRUE")
    set (INPUT_DICTIONARIES ${PROJECT_SOURCE_DIR}/input/empty.txt)
   else ()
    set (INPUT_DICTIONARIES "${PROJECT_SOURCE_DIR}/input/PoliMorf-0.6.7.tab,${PROJECT_SOURCE_DIR}/input/dodatki.tab")
   endif ()
endif ()

# INPUT_TAGSET
if ("${INPUT_TAGSET}" STREQUAL "")
   set (INPUT_TAGSET ${PROJECT_SOURCE_DIR}/input/polimorf.tagset)
endif ()

# SEGMENT_RULES_FILE
if ("${SEGMENT_RULES_FILE}" STREQUAL "")
   set (SEGMENT_RULES_FILE ${PROJECT_SOURCE_DIR}/input/segmenty.dat)
endif ()

message ("Will use ${INPUT_DICTIONARIES} as default dictionary input, ${INPUT_TAGSET} as tagset and ${SEGMENT_RULES_FILE} as segmentation rules")

# TARGET_DIR
if ("${TARGET_DIR}" STREQUAL "")
    set (TARGET_DIR "${PROJECT_BINARY_DIR}")
endif ()

### Compilation and linking flags

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++98 -Wall -pedantic -Wcast-align -Wextra -Wmissing-noreturn -Wconversion -Wcast-qual -Wcast-align")
    if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
    endif ()
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall -O2")
    set (CMAKE_SHARED_LIBRARY_PREFIX "")
    # here is some magic - must statically link libstdc++ and libgcc
    # but exclude libgcc_eh.a as it causes "multiple definition of (...)" errors
    set (CMAKE_SHARED_LINKER_FLAGS "-s -Os -static-libstdc++ -static-libgcc -Wl,--exclude-libs,libgcc_eh.a")
    set (CMAKE_EXE_LINKER_FLAGS "-s -Os -static-libstdc++ -static-libgcc")
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set (MACOSX_RPATH TRUE)
endif ()

########## Setup RPATH ##########

# use, i.e. don't skip the full RPATH for the build tree
set (CMAKE_SKIP_BUILD_RPATH  FALSE)

# when building, don't use the install RPATH already
# (but later on when installing)
set (CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 

set (CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
set (CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)


# the RPATH to be used when installing, but only if it's not a system directory
list (FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
if ("${isSystemDir}" STREQUAL "-1")
   SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
endif ()

########## Configure CPack ##########

# general
set (CPACK_PACKAGE_FILE_NAME "morfeusz-${PROJECT_VERSION}-${CMAKE_SYSTEM_NAME}-${ARCHITECTURE}")
set (CPACK_PACKAGE_VENDOR "Michał Lenart")
set (CPACK_PACKAGE_CONTACT "michal.lenart@ipipan.waw.pl")
set (CPACK_PACKAGE_DESCRIPTION_SUMMARY "Morphological analyzer for Polish language.")
set (CPACK_OUTPUT_FILE_PREFIX "${TARGET_DIR}")

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set (CPACK_GENERATOR "DEB" "TGZ")
    #debian
    set (CPACK_DEBIAN_PACKAGE_NAME "morfeusz")
    set (CPACK_DEBIAN_PACKAGE_MAINTAINER "${CPACK_PACKAGE_CONTACT}")
    set (CPACK_DEBIAN_PACKAGE_DEPENDS "libstdc++6 (>= 4.6)")
    set (CPACK_DEBIAN_PACKAGE_ARCHITECTURE "${ARCHITECTURE}")
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    if (${ARCHITECTURE} MATCHES "amd64")
        set (CPACK_GENERATOR "NSIS64" "TGZ")
    else ()
        set (CPACK_GENERATOR "NSIS" "TGZ")
    endif ()
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set (CPACK_GENERATOR "TGZ")
endif ()

include (CPack)

##### END initialize some vars #####
file (COPY fsabuilder testfiles input DESTINATION .)

configure_file (
  "${PROJECT_SOURCE_DIR}/morfeusz/MorfeuszConfig.hpp.in"
  "${PROJECT_BINARY_DIR}/morfeusz/MorfeuszConfig.hpp"
)

###### add main sources ########

include_directories("${PROJECT_BINARY_DIR}" )

add_subdirectory (morfeusz)
add_subdirectory (fsabuilder)

########## add tests ##########

macro (test_build_and_recognize fname method)
    add_test (TestBuild-${method}-${fname} python fsabuilder/buildfsa.py --analyzer --input-files testfiles/${fname} -o /tmp/test-${method}-${fname}.fsa --tagset-file=testfiles/polimorf.tagset --segments-file=testfiles/segmenty.dat --serialization-method=${method})
    add_test (TestBuild4Synth-${method}-${fname} python fsabuilder/buildfsa.py --generator --input-files testfiles/${fname} -o /tmp/test-synth-${method}-${fname}.fsa --tagset-file=testfiles/polimorf.tagset --serialization-method=${method})
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
