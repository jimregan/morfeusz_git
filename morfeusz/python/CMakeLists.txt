
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
    OUTPUT ${SWIG_PYTHON_OUTFILE} morfeusz.py
    COMMAND swig -python -c++ -o ${SWIG_PYTHON_OUTFILE} ${CMAKE_SOURCE_DIR}/morfeusz/morfeusz.i
    DEPENDS libmorfeusz
)
#set (CMAKE_SHARED_LINKER_FLAGS "-s -Os -static-libstdc++ -static-libgcc")
#add_library (pymorfeusz SHARED ${SWIG_PYTHON_OUTFILE})
#target_link_libraries (pymorfeusz ${PYTHON_LIBRARIES} libmorfeusz)
#add_dependencies (pymorfeusz ${SWIG_PYTHON_OUTFILE})
#set (CMAKE_SHARED_LIBRARY_PREFIX "")
#set_target_properties (pymorfeusz PROPERTIES OUTPUT_NAME "_morfeusz")

#~ if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
#~ 
    #~ set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D MS_WIN64")
    #~ set (CMAKE_SHARED_LIBRARY_SUFFIX ".pyd")
    #~ set (CMAKE_SHARED_LINKER_FLAGS "-s -Os -static-libstdc++ -static-libgcc -lpython27")
    #~ 
#~ elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
#~ 
    #~ set (CMAKE_SHARED_LIBRARY_SUFFIX ".so")
    #~ 
#~ endif ()

set(SETUP_PY_IN "${CMAKE_CURRENT_SOURCE_DIR}/setup.py.in")
set(SETUP_PY    "${CMAKE_CURRENT_BINARY_DIR}/setup.py")
set(DEPS        "${CMAKE_CURRENT_BINARY_DIR}/morfeusz.py" "${SWIG_PYTHON_OUTFILE}")
set(OUTPUT      "${CMAKE_CURRENT_BINARY_DIR}/build")

configure_file(${SETUP_PY_IN} ${SETUP_PY})

add_custom_command(OUTPUT ${OUTPUT}
                       COMMAND python
                       ARGS setup.py build
                       DEPENDS ${DEPS})

add_custom_target(pymorfeusz ALL DEPENDS ${OUTPUT})

set (CPACK_PACKAGE_NAME "dupa")
install (FILES setup.py.in DESTINATION bin)
include (CPack)
#install(CODE "execute_process(COMMAND python ${SETUP_PY} install --home=${CMAKE_INSTALL_PREFIX})")
