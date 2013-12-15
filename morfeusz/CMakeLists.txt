
# Make sure the linker can find the Hello library once it is built. 
#link_directories (${Morfeusz_BINARY_DIR}/Hello)

#include_directories (swig)

set(SRC_FILES 
    const.cpp 
    data/default_fsa.cpp
    MorphDeserializer.cpp 
    Tagset.cpp 
    fsa/const.cpp 
    MorphInterpretation.cpp 
    Morfeusz.cpp
    FlexionGraph.cpp
    charset/CharsetConverter.cpp
    charset/CaseConverter.cpp
    charset/caseconv.cpp
    charset/conversion_tables.cpp)

add_library (libmorfeusz SHARED ${SRC_FILES})
set_target_properties(libmorfeusz PROPERTIES OUTPUT_NAME "morfeusz")

add_executable (morfeusz main.cpp)
add_executable (test_result_equals test_result_equals.cpp)
add_executable (test_recognize_dict test_recognize_dict.cpp)

target_link_libraries (morfeusz libmorfeusz)
target_link_libraries (test_result_equals libmorfeusz)
target_link_libraries (test_recognize_dict libmorfeusz)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  target_link_libraries (libmorfeusz ws2_32)

  # here is the magic - must statically link libstdc++ and libgcc
  # but exclude libgcc_eh.a as it causes "multiple definition of (...)" errors

  set (CMAKE_SHARED_LINKER_FLAGS "-s -Os -static-libstdc++ -static-libgcc -Wl,--exclude-libs,libgcc_eh.a")
  set (CMAKE_EXE_LINKER_FLAGS "-s -Os -static-libstdc++ -static-libgcc")
  set (CMAKE_CXX_FLAGS "-std=c++0x -Wall -pedantic -Wcast-align -Wextra -Wmissing-noreturn -Wcast-qual -Wcast-align")
endif (${CMAKE_SYSTEM_NAME} MATCHES "Windows")

add_subdirectory (java)
add_subdirectory (python)
