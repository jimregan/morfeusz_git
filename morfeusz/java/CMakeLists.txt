# SWIG
#set(CMAKE_SWIG_OUTDIR swig)
#FIND_PACKAGE(SWIG REQUIRED)
FIND_PACKAGE(JNI REQUIRED)
#INCLUDE(${SWIG_USE_FILE})
include(UseJava)

# SWIG Java
include_directories (${JAVA_INCLUDE_PATH})
include_directories (${JAVA_INCLUDE_PATH2})
include_directories (..)

set (SWIG_JAVA_OUTFILE swigJAVA.cpp)
# set (JAVA_WRAPPER_FILE ${CMAKE_SHARED_LIBRARY_PREFIX}morfeusz${CMAKE_SHARED_LIBRARY_SUFFIX})
add_custom_command ( 
    OUTPUT ${SWIG_JAVA_OUTFILE}
    COMMAND swig -java -c++ -package pl.waw.ipipan.morfeusz  -o ${SWIG_JAVA_OUTFILE} -outdir ${CMAKE_SOURCE_DIR}/jmorfeusz/src/main/java/pl/waw/ipipan/morfeusz ${CMAKE_SOURCE_DIR}/morfeusz/morfeusz.i
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

# build jmorfeusz
add_custom_target ( jmorfeusz-maven-package package
    COMMAND mvn package -PotherOutputDir -DbuildDir=${CMAKE_CURRENT_BINARY_DIR} -DpackageDir=${PROJECT_BINARY_DIR} -DskipTests=true
    DEPENDS ${JMORFEUSZ}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/jmorfeusz)

#~ file(GLOB JMORFEUSZ_PACKAGE_FILES "${CMAKE_CURRENT_BINARY_DIR}/*.tar.gz")
#~ 
#~ add_custom_target(jmorfeusz-maven-package-copy ALL
    #~ DEPENDS jmorfeusz-maven-package)
#~ 
#~ foreach (f ${JMORFEUSZ_PACKAGE_FILES})
    #~ MESSAGE ("COPY ${f} TO ${PROJECT_BINARY_DIR}")
    #~ add_custom_command (
        #~ TARGET jmorfeusz-maven-package-copy
        #~ COMMAND ${CMAKE_COMMAND} -E copy ${f} ${PROJECT_BINARY_DIR}
        #~ 
    #~ )
#~ endforeach ()

