# SWIG
#set(CMAKE_SWIG_OUTDIR swig)
find_package (SWIG REQUIRED)
include (${SWIG_USE_FILE})
set (JMORFEUSZ_VERSION "0.1.0")
find_package(JNI REQUIRED)
#find_package(JavaLibs REQUIRED)
#INCLUDE(${SWIG_USE_FILE})
include(UseJava)
find_package(Java REQUIRED)

# SWIG Java
include_directories (${JAVA_INCLUDE_PATH})
include_directories (${JAVA_INCLUDE_PATH2})
include_directories (..)

set (SWIG_JAVA_OUTFILE swigJAVA.cpp)
file (COPY pl DESTINATION .)
set (JAVA_SRC_DIR "${CMAKE_CURRENT_BINARY_DIR}/pl/waw/ipipan/morfeusz")
add_custom_target (
    libjmorfeusz_cxx
#    OUTPUT ${SWIG_JAVA_OUTFILE}
    COMMAND swig -java -c++ -package pl.waw.ipipan.morfeusz  -o ${SWIG_JAVA_OUTFILE} -outdir ${JAVA_SRC_DIR} ${CMAKE_SOURCE_DIR}/morfeusz/morfeusz.i
    DEPENDS libmorfeusz
)

add_library (libjmorfeusz SHARED ${SWIG_JAVA_OUTFILE})
set_target_properties (libjmorfeusz PROPERTIES OUTPUT_NAME "jmorfeusz")
target_link_libraries (libjmorfeusz ${JAVA_LIBRARIES} libmorfeusz)
add_dependencies (libjmorfeusz libjmorfeusz_cxx)

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
#~ add_custom_target (jmorfeusz-repair-library
    #~ COMMAND ${DARWIN64_ROOT}/x86_64-apple-darwin9/bin/x86_64-apple-darwin9-install_name_tool -change /home/mlenart/xxx/morfeusz/buildall/build-Darwin-amd64/morfeusz/libmorfeusz2.dylib morfeusz2 ${PROJECT_BINARY_DIR}/morfeusz/java/libjmorfeusz.jnilib
    #~ DEPENDS libjmorfeusz)

# build jmorfeusz
file(GLOB_RECURSE JAVA_SOURCES ${JAVA_SRC_DIR} "*.java")
add_jar (jmorfeusz
    SOURCES "${JAVA_SOURCES}"
    DEPENDS libjmorfeusz)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    add_dependencies(jmorfeusz jmorfeusz-repair-libmorfeusz-link jmorfeusz-repair-libjmorfeusz-id)
endif ()

add_custom_target (jmorfeusz-copy-readme
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/README" "${CMAKE_CURRENT_BINARY_DIR}/README")


add_custom_target(package-java
    COMMAND mkdir -p "${TARGET_DIR}" && ${CMAKE_COMMAND} -E tar "cfvz" "${TARGET_DIR}/jmorfeusz-${JMORFEUSZ_VERSION}-${CMAKE_SYSTEM_NAME}-${ARCHITECTURE}.tar.gz" "${CMAKE_CURRENT_BINARY_DIR}/*.jar" "${CMAKE_CURRENT_BINARY_DIR}/*${CMAKE_SHARED_LIBRARY_SUFFIX}" "${CMAKE_CURRENT_BINARY_DIR}/README"
    DEPENDS jmorfeusz jmorfeusz-copy-readme libjmorfeusz)
