
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
set_target_properties (libmorfeusz PROPERTIES OUTPUT_NAME "morfeusz")

add_executable (morfeusz main.cpp)
add_executable (test_result_equals test_result_equals.cpp)
add_executable (test_recognize_dict test_recognize_dict.cpp)

target_link_libraries (morfeusz libmorfeusz)
target_link_libraries (test_result_equals libmorfeusz)
target_link_libraries (test_recognize_dict libmorfeusz)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  target_link_libraries (libmorfeusz ws2_32)
endif ()

add_subdirectory (java)
add_subdirectory (python)
