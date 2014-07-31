
set (JMORFEUSZ_VERSION "0.1.0")
find_package(JNI REQUIRED)
include(UseJava)
find_package(Java REQUIRED)

include_directories (${JAVA_INCLUDE_PATH})
include_directories (${JAVA_INCLUDE_PATH2})
include_directories (${CMAKE_SOURCE_DIR}/morfeusz)

set (SWIG_JAVA_OUTFILE "${CMAKE_CURRENT_BINARY_DIR}/swigJAVA.cpp")
file (COPY pl DESTINATION .)
set (JAVA_SRC_DIR "${CMAKE_CURRENT_BINARY_DIR}/pl/waw/ipipan/morfeusz")
add_custom_command (
    OUTPUT ${SWIG_JAVA_OUTFILE}
    COMMAND swig -java -c++ -package pl.waw.ipipan.morfeusz  -o ${SWIG_JAVA_OUTFILE} -outdir ${JAVA_SRC_DIR} ${CMAKE_SOURCE_DIR}/morfeusz/wrappers/morfeusz.i
    DEPENDS libmorfeusz
)
add_custom_target(generate-java-wrapper ALL
    DEPENDS ${SWIG_JAVA_OUTFILE})
add_library (libjmorfeusz SHARED ${SWIG_JAVA_OUTFILE})
add_dependencies (libjmorfeusz generate_java_wrapper)
set_target_properties (libjmorfeusz PROPERTIES OUTPUT_NAME "jmorfeusz")
target_link_libraries (libjmorfeusz ${JAVA_LIBRARIES} libmorfeusz)

if ( CMAKE_COMPILER_IS_GNUCC )
    set_property( TARGET libjmorfeusz APPEND_STRING PROPERTY COMPILE_FLAGS -w )
endif ( CMAKE_COMPILER_IS_GNUCC )

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set (CMAKE_SHARED_LINKER_FLAGS "-s -Os -static-libstdc++ -static-libgcc -D_JNI_IMPLEMENTATION_ -Wl,--kill-at")
endif ()

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set (CMAKE_SHARED_LIBRARY_SUFFIX ".jnilib")
    #~ set (CMAKE_SHARED_LINKER_FLAGS "${-dylib")
endif ()

set (CMAKE_JAVA_TARGET_VERSION ${JMORFEUSZ_VERSION})
set (CMAKE_JAVA_TARGET_OUTPUT_NAME jmorfeusz)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    get_filename_component (LIBMORFEUSZ_DIR ${CMAKE_CURRENT_BINARY_DIR} DIRECTORY)
    add_custom_target (jmorfeusz-repair-libmorfeusz-link
        COMMAND ${INSTALL_NAME_TOOL} -change ${LIBMORFEUSZ_DIR}/libmorfeusz2.dylib libmorfeusz2.dylib ${CMAKE_CURRENT_BINARY_DIR}/libjmorfeusz.jnilib
        DEPENDS libjmorfeusz)
    add_custom_target (jmorfeusz-repair-libjmorfeusz-id
        COMMAND ${INSTALL_NAME_TOOL} -id libjmorfeusz.jnilib ${CMAKE_CURRENT_BINARY_DIR}/libjmorfeusz.jnilib
        DEPENDS libjmorfeusz jmorfeusz-repair-libmorfeusz-link)
endif ()

# build jmorfeusz
file(GLOB_RECURSE JAVA_SOURCES ${JAVA_SRC_DIR} "*.java")
add_jar (jmorfeusz
    SOURCES "${JAVA_SOURCES}"
    ENTRY_POINT pl/waw/ipipan/morfeusz/app/App
    DEPENDS libjmorfeusz generate-java-wrapper)

#create_javadoc(jmorfeusz_javadoc
#  PACKAGES pl.waw.ipipan.morfeusz
#  SOURCEPATH "${CMAKE_CURRENT_BINARY_DIR}"
#  CLASSPATH "${CMAKE_JAVA_INCLUDE_PATH}"
#  WINDOWTITLE "JMorfeusz - Morfeusz Java binding"
#  DOCTITLE "<h1>JMorfeusz</h1>"
#  AUTHOR TRUE
#  USE TRUE
#  VERSION TRUE
#  DEPENDS jmorfeusz generate_java_wrapper)

add_custom_target(generate-javadoc ALL
    COMMAND javadoc
        -d "${CMAKE_CURRENT_BINARY_DIR}/jmorfeusz_javadoc"
        -sourcepath "${CMAKE_CURRENT_BINARY_DIR}"
        -windowtitle "JMorfeusz - Morfeusz Java binding"
        -use
        -link http://docs.oracle.com/javase/7/docs/api/
        pl.waw.ipipan.morfeusz
    DEPENDS jmorfeusz)

add_custom_target(package-javadoc ALL
    COMMAND mkdir -p "${TARGET_DIR}" && ${CMAKE_COMMAND} -E tar "cfvz" "${TARGET_DIR}/jmorfeusz_javadoc.tgz" "${CMAKE_CURRENT_BINARY_DIR}/jmorfeusz_javadoc"
    DEPENDS generate-javadoc)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    add_dependencies(jmorfeusz jmorfeusz-repair-libmorfeusz-link jmorfeusz-repair-libjmorfeusz-id)
endif ()

add_custom_target (jmorfeusz-copy-readme
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/README" "${CMAKE_CURRENT_BINARY_DIR}/README")


add_custom_target(package-java
    COMMAND mkdir -p "${TARGET_DIR}" && ${CMAKE_COMMAND} -E tar "cfvz" "${TARGET_DIR}/jmorfeusz-${JMORFEUSZ_VERSION}-${CMAKE_SYSTEM_NAME}-${ARCHITECTURE}.tar.gz" "${CMAKE_CURRENT_BINARY_DIR}/*.jar" "${CMAKE_CURRENT_BINARY_DIR}/*${CMAKE_SHARED_LIBRARY_SUFFIX}" "${CMAKE_CURRENT_BINARY_DIR}/README"
    DEPENDS jmorfeusz package-javadoc jmorfeusz-copy-readme libjmorfeusz)
