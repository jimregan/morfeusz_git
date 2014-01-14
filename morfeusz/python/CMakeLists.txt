
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
        COMMAND python ${SETUP_PY} bdist_egg -d "${TARGET_DIR}/python" --plat-name "${CMAKE_SYSTEM_NAME}-${ARCHITECTURE}"
        DEPENDS pymorfeusz
    )
    set (PACKAGE_DEPENDS "package-python-bin")
    
    if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        add_custom_target (package-python-deb-sdist
            COMMAND python ${SETUP_PY} --command-packages=stdeb.command sdist_dsc --depends "${CPACK_DEBIAN_PACKAGE_NAME}"
            DEPENDS pymorfeusz
        )
        add_custom_target (package-python-deb-build
            COMMAND debuild -us -uc
            WORKING_DIRECTORY deb_dist/pymorfeusz-${PYMORFEUSZ_VERSION}
            DEPENDS package-python-deb-sdist
        )
        add_custom_target(package-python-deb
            COMMAND mkdir -p "${TARGET_DIR}/python" && cp ${CMAKE_CURRENT_BINARY_DIR}/deb_dist/python-pymorfeusz*.deb "${TARGET_DIR}/python"
            DEPENDS package-python-deb-build
        )
        list (APPEND PACKAGE_DEPENDS package-python-deb)
    elseif (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
        add_custom_target (package-python-win-installer
            COMMAND python ${SETUP_PY} bdist_wininst -d ${TARGET_DIR} --plat-name "${CMAKE_SYSTEM_NAME}-${ARCHITECTURE}"
            DEPENDS pymorfeusz
        )
        list (APPEND PACKAGE_DEPENDS package-python-win-installer)
    endif ()
    message ("PACKAGE_DEPENDS=${PACKAGE_DEPENDS}")
    add_custom_target (package-python
        DEPENDS pymorfeusz ${PACKAGE_DEPENDS})
endif ()
