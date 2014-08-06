

########## generate default dictionary data #################

if (SKIP_DICTIONARY_BUILDING)
    message ("SKIPPING dictionary building")
else ()
    add_custom_command (
            OUTPUT "${ANALYZER_DICTIONARY_CPP}"
            COMMAND python ${PROJECT_SOURCE_DIR}/fsabuilder/morfeusz_builder --analyzer --input-files="${INPUT_DICTIONARIES}" -o "${ANALYZER_DICTIONARY_CPP}" --tagset-file="${INPUT_TAGSET}" --segments-file="${SEGMENT_RULES_FILE}" --cpp --serialization-method=V1
            DEPENDS "${INPUT_DICTIONARY}"
            COMMENT "Building default dictionary C++ file"
    )
    add_custom_command (
            OUTPUT "${GENERATOR_DICTIONARY_CPP}"
            COMMAND python ${PROJECT_SOURCE_DIR}/fsabuilder/morfeusz_builder --generator --input-files="${INPUT_DICTIONARIES}" -o "${GENERATOR_DICTIONARY_CPP}" --tagset-file="${INPUT_TAGSET}" --segments-file="${SEGMENT_RULES_FILE}" --cpp --serialization-method=V1
            DEPENDS "${INPUT_DICTIONARY}"
            COMMENT "Building default dictionary C++ file"
    )
endif()

add_custom_target ( analyzer-dictionary DEPENDS "${INPUT_DICTIONARY_CPP}")
add_custom_target ( generator-dictionary DEPENDS "${INPUT_SYNTH_DICTIONARY_CPP}")
add_custom_target ( dictionaries DEPENDS analyzer-dictionary generator-dictionary)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

#### build #####

set(SRC_FILES 
    const.cpp
    ${ANALYZER_DICTIONARY_CPP}
    ${GENERATOR_DICTIONARY_CPP}
    Environment.cpp
    IdResolverImpl.cpp
    fsa/const.cpp 
    MorphInterpretation.cpp 
    Morfeusz.cpp
    MorfeuszInternal.cpp
    ResultsIteratorImpl.cpp
    InflexionGraph.cpp
    charset/TextReader.cpp
    charset/CharsetConverter.cpp
    case/CaseConverter.cpp
    case/caseconv.cpp
    charset/conversion_tables.cpp
    cli/cli.cpp
    segrules/segrules.cpp
    segrules/SegrulesFSA.cpp
    case/CasePatternHelper.cpp
    deserialization/morphInterps/InterpretedChunksDecoder.cpp
    deserialization/morphInterps/InterpretedChunksDecoder4Analyzer.cpp
    deserialization/morphInterps/InterpretedChunksDecoder4Generator.cpp
    deserialization/InterpsGroupsReader.cpp
    deserialization/MorphDeserializer.cpp
    morfeusz2_c.cpp
    c_api/ResultsManager.cpp
)

file (GLOB TEST_SRC_FILES tests/*.cpp)

add_library (libmorfeusz SHARED ${SRC_FILES})
set_source_files_properties ( SOURCE "${ANALYZER_DICTIONARY_CPP}" PROPERTIES GENERATED TRUE)
set_source_files_properties ( SOURCE "${GENERATOR_DICTIONARY_CPP}" PROPERTIES GENERATED TRUE)
set_target_properties (libmorfeusz PROPERTIES OUTPUT_NAME "morfeusz2")

add_executable (morfeusz_analyzer morfeusz_analyzer.cpp)
add_executable (morfeusz_generator morfeusz_generator.cpp)

target_link_libraries (morfeusz_analyzer libmorfeusz)
target_link_libraries (morfeusz_generator libmorfeusz)

add_subdirectory (wrappers)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    add_custom_target (morfeusz-repair-library
        COMMAND ${INSTALL_NAME_TOOL} -id libmorfeusz2.dylib ${CMAKE_CURRENT_BINARY_DIR}/libmorfeusz2.dylib
        DEPENDS libmorfeusz)
    add_dependencies (morfeusz_analyzer morfeusz-repair-library)
endif ()

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    target_link_libraries (libmorfeusz ws2_32)
    set (TARGET_LIB_DIR bin)
else ()
    set (TARGET_LIB_DIR lib)
endif ()

install (FILES morfeusz2.h morfeusz2_c.h DESTINATION include)
install (TARGETS libmorfeusz DESTINATION ${TARGET_LIB_DIR})
install (TARGETS morfeusz_analyzer morfeusz_generator DESTINATION bin)

# TESTING

if (NOT CMAKE_CROSSCOMPILING AND NOT SKIP_TESTING)
    find_library(CPPUNIT_LIB cppunit)
    if(${CPPUNIT_LIB} STREQUAL "CPPUNIT_LIB-NOTFOUND")
        message(FATAL_ERROR "Couldn't find cppunit library. Please install cppunit or add -D SKIP_TESTING=1 option.")
    endif()
    add_executable (test_runner test_runner.cpp ${TEST_SRC_FILES})
    target_link_libraries (test_runner ${CPPUNIT_LIB} libmorfeusz)
    add_custom_target (build-tests DEPENDS test_runner)
    add_test(CPPUnitTests test_runner)
endif ()
