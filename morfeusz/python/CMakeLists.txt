
# SWIG
#set(CMAKE_SWIG_OUTDIR swig)
FIND_PACKAGE (SWIG REQUIRED)
FIND_PACKAGE (PythonLibs)
INCLUDE (${SWIG_USE_FILE})

set (PYMORFEUSZ_VERSION "0.1.0")

# SWIG Java
INCLUDE_DIRECTORIES (${PYTHON_INCLUDE_PATH})
INCLUDE_DIRECTORIES (..)

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

set (SETUP_PY_IN "${CMAKE_CURRENT_SOURCE_DIR}/setup.py.in")
set (SETUP_PY    "${CMAKE_CURRENT_BINARY_DIR}/setup.py")
set (DEPS        "${CMAKE_CURRENT_BINARY_DIR}/morfeusz.py" "${SWIG_PYTHON_OUTFILE}")
set (OUTPUT      "${CMAKE_CURRENT_BINARY_DIR}/build")

configure_file (${SETUP_PY_IN} ${SETUP_PY})

add_custom_command (OUTPUT ${OUTPUT}
                       COMMAND python
                       ARGS setup.py build
                       DEPENDS ${DEPS})

add_custom_target (pymorfeusz ALL DEPENDS ${OUTPUT} morfeusz)


#~ install(CODE "execute_process(COMMAND python ${SETUP_PY} install --home=${CMAKE_INSTALL_PREFIX})" COMPONENT pythonapi DEPENDS ${OUTPUT} MESSAGE "DUPA")
#~ install(CODE "execute_process(COMMAND python ${SETUP_PY} --command-packages=stdeb.command bdist_deb)" COMPONENT pythondeb DEPENDS ${OUTPUT})
#~ install(CODE "execute_process(COMMAND python ${SETUP_PY} bdist -d ${PROJECT_BINARY_DIR})" COMPONENT pythontgz DEPENDS ${OUTPUT})

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    add_custom_target (install-python
        COMMAND python ${SETUP_PY} install --home=${CMAKE_INSTALL_PREFIX}
        DEPENDS ${pymorfeusz}
    )
    add_custom_target (package-python-bin
        COMMAND python ${SETUP_PY} bdist -d ${PROJECT_BINARY_DIR}
        DEPENDS pymorfeusz
    )
    add_custom_target (package-python-src
        COMMAND python ${SETUP_PY} sdist -d ${PROJECT_BINARY_DIR}
        DEPENDS pymorfeusz
    )
    add_custom_target (package-python
        DEPENDS package-python-bin package-python-src)
    
    set (PYMORFEUSZ_SRC_PACKAGE "${PROJECT_BINARY_DIR}/pymorfeusz-${PYMORFEUSZ_VERSION}.tar.gz")
    add_custom_target (package-python-deb-src
        COMMAND py2dsc ${PYMORFEUSZ_SRC_PACKAGE}
        DEPENDS package-python-src
    )
endif ()
#~ install (install-python 
    #~ COMMAND python ${SETUP_PY} install --home=${CMAKE_INSTALL_PREFIX} 
    #~ DEPENDS pymorfeusz)

#~ install (CODE
    #~ COMMAND python ${SETUP_PY} install --home=${CMAKE_INSTALL_PREFIX} 
    #~ DEPENDS pymorfeusz)
