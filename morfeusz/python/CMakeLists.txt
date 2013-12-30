
# SWIG
#set(CMAKE_SWIG_OUTDIR swig)
FIND_PACKAGE(SWIG REQUIRED)
FIND_PACKAGE(PythonLibs)
INCLUDE(${SWIG_USE_FILE})

# SWIG Java
INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})
INCLUDE_DIRECTORIES(..)

set (SWIG_PYTHON_OUTFILE swigPYTHON.cpp)
# set (JAVA_WRAPPER_FILE ${CMAKE_SHARED_LIBRARY_PREFIX}morfeusz${CMAKE_SHARED_LIBRARY_SUFFIX})
add_custom_command ( 
    OUTPUT ${SWIG_PYTHON_OUTFILE}
    COMMAND swig -python -c++ -o ${SWIG_PYTHON_OUTFILE} ${CMAKE_SOURCE_DIR}/morfeusz/morfeusz.i
    DEPENDS libmorfeusz
)
#set (CMAKE_SHARED_LINKER_FLAGS "-s -Os -static-libstdc++ -static-libgcc")
add_library (pymorfeusz SHARED ${SWIG_PYTHON_OUTFILE})
target_link_libraries (pymorfeusz ${PYTHON_LIBRARIES} libmorfeusz)
add_dependencies (pymorfeusz ${SWIG_PYTHON_OUTFILE})
set (CMAKE_SHARED_LIBRARY_PREFIX "")
set_target_properties (pymorfeusz PROPERTIES OUTPUT_NAME "_morfeusz")

#set (CMAKE_SWIG_FLAGS -package pl.waw.ipipan.morfeusz)
#set (CMAKE_SWIG_OUTDIR ${CMAKE_SOURCE_DIR}/jmorfeusz/src/main/java/pl/waw/ipipan/morfeusz) 

#set_source_files_properties (../morfeusz.i PROPERTIES CPLUSPLUS ON)
#SWIG_ADD_MODULE(jmorfeusz java ../morfeusz.i)
#SWIG_LINK_LIBRARIES(jmorfeusz ${JAVA_LIBRARIES})
#SWIG_LINK_LIBRARIES(jmorfeusz libmorfeusz)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")

    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D MS_WIN64")
    set (CMAKE_SHARED_LIBRARY_SUFFIX ".pyd")
    set (CMAKE_SHARED_LINKER_FLAGS "-s -Os -static-libstdc++ -static-libgcc -lpython27")
    
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

    set (CMAKE_SHARED_LIBRARY_SUFFIX ".so")
    
endif ()

#~ SET(CMAKE_SWIG_FLAGS "")
#~ 
#~ SET_SOURCE_FILES_PROPERTIES(../morfeusz.i PROPERTIES CPLUSPLUS ON)
#~ SWIG_ADD_MODULE(morfeusz python ../morfeusz.i)
#~ SWIG_LINK_LIBRARIES(morfeusz ${PYTHON_LIBRARIES})
#~ SWIG_LINK_LIBRARIES(morfeusz libmorfeusz)
#~ 
#~ if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    #~ set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D MS_WIN64")
#~ endif ()
