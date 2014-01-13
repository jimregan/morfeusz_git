# SWIG
#set(CMAKE_SWIG_OUTDIR swig)
#FIND_PACKAGE(SWIG REQUIRED)
set (JMORFEUSZ_VERSION "0.1.0")
find_package(JNI REQUIRED)
#INCLUDE(${SWIG_USE_FILE})
include(UseJava)
find_package(Java REQUIRED)

# SWIG Java
include_directories (${JAVA_INCLUDE_PATH})
include_directories (${JAVA_INCLUDE_PATH2})
include_directories (..)

set (SWIG_JAVA_OUTFILE swigJAVA.cpp)
set (JAVA_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/pl/waw/ipipan/morfeusz")
# set (JAVA_WRAPPER_FILE ${CMAKE_SHARED_LIBRARY_PREFIX}morfeusz${CMAKE_SHARED_LIBRARY_SUFFIX})
add_custom_command ( 
    OUTPUT ${SWIG_JAVA_OUTFILE}
    COMMAND swig -java -c++ -package pl.waw.ipipan.morfeusz  -o ${SWIG_JAVA_OUTFILE} -outdir ${JAVA_SRC_DIR} ${CMAKE_SOURCE_DIR}/morfeusz/morfeusz.i
    DEPENDS libmorfeusz
)
#set (CMAKE_SHARED_LINKER_FLAGS "-s -Os -static-libstdc++ -static-libgcc")
add_library (jmorfeusz SHARED ${SWIG_JAVA_OUTFILE})
target_link_libraries (jmorfeusz ${JAVA_LIBRARIES} libmorfeusz)
add_dependencies (jmorfeusz ${SWIG_JAVA_OUTFILE})

#set (CMAKE_SWIG_FLAGS -package pl.waw.ipipan.morfeusz)
#set (CMAKE_SWIG_OUTDIR ${CMAKE_SOURCE_DIR}/jmorfeusz/src/main/java/pl/waw/ipipan/morfeusz) 

#set_source_files_properties (../morfeusz.i PROPERTIES CPLUSPLUS ON)
#SWIG_ADD_MODULE(jmorfeusz java ../morfeusz.i)
#SWIG_LINK_LIBRARIES(jmorfeusz ${JAVA_LIBRARIES})
#SWIG_LINK_LIBRARIES(jmorfeusz libmorfeusz)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set (CMAKE_SHARED_LINKER_FLAGS "-s -Os -static-libstdc++ -static-libgcc -D_JNI_IMPLEMENTATION_ -Wl,--kill-at")
endif ()

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set (CMAKE_SHARED_LIBRARY_SUFFIX ".jnilib")
endif ()

set (CMAKE_JAVA_TARGET_VERSION ${JMORFEUSZ_VERSION})
set (CMAKE_JAVA_TARGET_OUTPUT_NAME jmorfeusz)

# build jmorfeusz
add_jar (jmorfeusz-jar 
    SOURCES ${JAVA_SRC_DIR}/*.java ${JAVA_SRC_DIR}/app/*.java )

add_custom_target(jmorfeusz-copy-readme
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/README" "${CMAKE_CURRENT_BINARY_DIR}/README")

add_custom_target(package-java
    COMMAND ${CMAKE_COMMAND} -E tar "cfvz" "${PROJECT_BINARY_DIR}/jmorfeusz-${JMORFEUSZ_VERSION}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}.tgz" "${CMAKE_CURRENT_BINARY_DIR}/*.jar" "${CMAKE_CURRENT_BINARY_DIR}/*${CMAKE_SHARED_LIBRARY_SUFFIX}" "${CMAKE_CURRENT_BINARY_DIR}/README"
    DEPENDS jmorfeusz-jar jmorfeusz-copy-readme jmorfeusz)
