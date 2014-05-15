

########## generate default dictionary data #################
add_custom_command (
        OUTPUT "${INPUT_DICTIONARY_CPP}"
        COMMAND python ${PROJECT_SOURCE_DIR}/fsabuilder/morfeusz_builder --analyzer --input-files="${INPUT_DICTIONARIES}" -o "${INPUT_DICTIONARY_CPP}" --tagset-file="${INPUT_TAGSET}" --segments-file="${SEGMENT_RULES_FILE}" --cpp --serialization-method=V1 --trim-supneg
        DEPENDS "${INPUT_DICTIONARY}"
        COMMENT "Building default dictionary C++ file"
)
add_custom_command (
        OUTPUT "${INPUT_SYNTH_DICTIONARY_CPP}"
        COMMAND python ${PROJECT_SOURCE_DIR}/fsabuilder/morfeusz_builder --generator --input-files="${INPUT_DICTIONARIES}" -o "${INPUT_SYNTH_DICTIONARY_CPP}" --tagset-file="${INPUT_TAGSET}" --segments-file="${SEGMENT_RULES_FILE}" --cpp --serialization-method=V1
        DEPENDS "${INPUT_DICTIONARY}"
        COMMENT "Building default dictionary C++ file"
)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

# add_custom_target (dupa DEPENDS "${INPUT_DICTIONARY_CPP}")

#### build #####

set(SRC_FILES 
    const.cpp
    ${INPUT_DICTIONARY_CPP}
    ${INPUT_SYNTH_DICTIONARY_CPP}
    Environment.cpp
    MorphDeserializer.cpp
    Tagset.cpp
    Qualifiers.cpp
    fsa/const.cpp 
    MorphInterpretation.cpp 
    Morfeusz.cpp
    InflexionGraph.cpp
    charset/CharsetConverter.cpp
    charset/CaseConverter.cpp
    charset/caseconv.cpp
    charset/conversion_tables.cpp
    cli/cli.cpp
    segrules/segrules.cpp
    segrules/SegrulesFSA.cpp
    CasePatternHelper.cpp
    decoder/InterpretedChunksDecoder4Analyzer.cpp
    decoder/InterpretedChunksDecoder4Generator.cpp
)

set(INCLUDE_FILES 
    const.hpp 
    data/default_fsa.hpp
    MorphDeserializer.hpp
    Tagset.hpp
    Qualifiers.hpp
    fsa/const.hpp 
    MorphInterpretation.hpp 
    Morfeusz.hpp
    InflexionGraph.hpp
    charset/CharsetConverter.hpp
    charset/CaseConverter.hpp
    charset/caseconv.hpp
    charset/conversion_tables.hpp
    cli/cli.hpp
    segrules/segrules.hpp
)

add_library (libmorfeusz SHARED ${SRC_FILES})
set_source_files_properties ( SOURCE "${INPUT_DICTIONARY_CPP}" PROPERTIES GENERATED TRUE)
set_target_properties (libmorfeusz PROPERTIES OUTPUT_NAME "morfeusz2")

add_executable (morfeusz_analyzer morfeusz_analyzer.cpp)
add_executable (morfeusz_generator morfeusz_generator.cpp)
add_executable (test_result_equals test_result_equals.cpp)
add_executable (test_recognize_dict test_recognize_dict.cpp)

target_link_libraries (morfeusz_analyzer libmorfeusz)
target_link_libraries (morfeusz_generator libmorfeusz)
target_link_libraries (test_result_equals libmorfeusz)
target_link_libraries (test_recognize_dict libmorfeusz)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    target_link_libraries (libmorfeusz ws2_32)
    set (TARGET_LIB_DIR bin)
else ()
    set (TARGET_LIB_DIR lib)
endif ()

add_subdirectory (java)
add_subdirectory (python)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    add_custom_target (morfeusz-repair-library
        COMMAND ${INSTALL_NAME_TOOL} -id libmorfeusz2.dylib ${CMAKE_CURRENT_BINARY_DIR}/libmorfeusz2.dylib
        DEPENDS libmorfeusz)
    add_dependencies (morfeusz_analyzer morfeusz-repair-library)
endif ()

install (FILES ${INCLUDE_FILES} DESTINATION include/morfeusz)
install (TARGETS libmorfeusz DESTINATION ${TARGET_LIB_DIR})
install (TARGETS morfeusz_analyzer morfeusz_generator DESTINATION bin)
