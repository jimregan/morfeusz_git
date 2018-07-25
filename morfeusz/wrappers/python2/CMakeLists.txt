
# SWIG
#set(CMAKE_SWIG_OUTDIR swig)
#~ FIND_PACKAGE (SWIG REQUIRED)
FIND_PACKAGE (PythonLibs ${PY} EXACT)
#~ INCLUDE (${SWIG_USE_FILE})

set (PY2MORFEUSZ_VERSION "0.4.0")

# SWIG Java
INCLUDE_DIRECTORIES (${PYTHON_INCLUDE_PATH})
INCLUDE_DIRECTORIES (../..)


set (SWIG_PYTHON_OUTFILE_CXX "${CMAKE_CURRENT_BINARY_DIR}/swigPYTHON.cpp")
set (SWIG_PYTHON_OUTFILE_PY "${CMAKE_CURRENT_BINARY_DIR}/morfeusz2.py")
add_custom_command ( 
    OUTPUT "${SWIG_PYTHON_OUTFILE_CXX}" "${SWIG_PYTHON_OUTFILE_PY}"
    COMMAND swig -python -c++ -o "${SWIG_PYTHON_OUTFILE_CXX}" "${CMAKE_SOURCE_DIR}/morfeusz/wrappers/morfeuszpy2.i"
    DEPENDS libmorfeusz
)
add_custom_target (generate_python2_wrapper ALL
        DEPENDS "${SWIG_PYTHON_OUTFILE_CXX}" "${SWIG_PYTHON_OUTFILE_PY}")

set (SETUP_PY_IN "${CMAKE_CURRENT_SOURCE_DIR}/setup.py.in")
set (SETUP_PY_TMP    "${CMAKE_CURRENT_BINARY_DIR}/setupTmp.py")
set (SETUP_PY    "${CMAKE_CURRENT_BINARY_DIR}/setup.py")
set (DEPS        "${SWIG_PYTHON_OUTFILE_CXX}" "${SWIG_PYTHON_OUTFILE_PY}")
set (OUTPUT      "${CMAKE_CURRENT_BINARY_DIR}/build")

configure_file (${SETUP_PY_IN} ${SETUP_PY})

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/morfeusz2-0.4.0-py2.7" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

add_custom_target (generate_python2_setup_file
        DEPENDS ${SETUP_PY})

add_custom_command (OUTPUT ${OUTPUT}
                       COMMAND python2
                       ARGS ${SETUP_PY} build
                       DEPENDS generate_python2_wrapper generate_python2_setup_file libmorfeusz)

add_custom_target (build_python2_wrapper
        DEPENDS ${OUTPUT})

add_custom_target (py2morfeusz
        DEPENDS generate_python2_wrapper generate_python2_setup_file libmorfeusz build_python2_wrapper)

if (UNIX)
    add_custom_target (install-python2
        COMMAND python2 ${SETUP_PY} install --home=${CMAKE_INSTALL_PREFIX}
        DEPENDS py2morfeusz
    )
    add_custom_target (package-python2-bin
        COMMAND python2 ${SETUP_PY} bdist_egg -d "${TARGET_DIR}" --plat-name "${CMAKE_SYSTEM_NAME}-${ARCHITECTURE}"
        DEPENDS py2morfeusz
    )
    set (PACKAGE_DEPENDS "package-python2-bin")
    
    if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        add_custom_target (package-python2-deb-sdist
            COMMAND python2 ${SETUP_PY} --command-packages=stdeb.command sdist_dsc --depends "${CPACK_DEBIAN_PACKAGE_NAME}" --copyright-file "${PROJECT_SOURCE_DIR}/License.txt"
            DEPENDS py2morfeusz
        )
        add_custom_target (package-python2-deb-build
            COMMAND debuild -us -uc
            WORKING_DIRECTORY deb_dist/morfeusz2-${PY2MORFEUSZ_VERSION}
            DEPENDS package-python2-deb-sdist
        )
        add_custom_target(package-python2-deb
            COMMAND mkdir -p "${TARGET_DIR}" && cp ${CMAKE_CURRENT_BINARY_DIR}/deb_dist/python-morfeusz2*.deb "${TARGET_DIR}"
            DEPENDS package-python2-deb-build
        )
        list (APPEND PACKAGE_DEPENDS package-python2-deb)
    endif ()
    add_custom_target (package-python2
        DEPENDS py2morfeusz ${PACKAGE_DEPENDS})
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Windows" AND NOT CMAKE_CROSSCOMPILING)
    add_custom_target (package-python2-win-egg
        COMMAND python2 ${SETUP_PY} bdist_egg -d ${TARGET_DIR} --plat-name "${CMAKE_SYSTEM_NAME}-${ARCHITECTURE}"
        DEPENDS py2morfeusz
    )
    add_custom_target (package-python2-win-installer
        COMMAND python2 ${SETUP_PY} bdist_wininst -d ${TARGET_DIR} --plat-name "${CMAKE_SYSTEM_NAME}-${ARCHITECTURE}"
        DEPENDS py2morfeusz
    )
    list (APPEND PACKAGE_DEPENDS package-python2-win-installer)
    add_custom_target (package-python2
        DEPENDS py2morfeusz ${PACKAGE_DEPENDS})
endif ()
