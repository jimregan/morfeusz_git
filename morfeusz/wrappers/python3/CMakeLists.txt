
# SWIG
#set(CMAKE_SWIG_OUTDIR swig)
#~ FIND_PACKAGE (SWIG REQUIRED)
FIND_PACKAGE (PythonLibs 3.0 REQUIRED)
#~ INCLUDE (${SWIG_USE_FILE})

set (PY3MORFEUSZ_VERSION "0.4.0")

# SWIG Java
INCLUDE_DIRECTORIES (${PYTHON_INCLUDE_PATH})
INCLUDE_DIRECTORIES (../..)


set (SWIG_PYTHON_OUTFILE_CXX "${CMAKE_CURRENT_BINARY_DIR}/swigPYTHON.cpp")
set (SWIG_PYTHON_OUTFILE_PY "${CMAKE_CURRENT_BINARY_DIR}/morfeusz2.py")
add_custom_command ( 
    OUTPUT "${SWIG_PYTHON_OUTFILE_CXX}" "${SWIG_PYTHON_OUTFILE_PY}"
    COMMAND swig -python -py3 -c++ -o "${SWIG_PYTHON_OUTFILE_CXX}" "${CMAKE_SOURCE_DIR}/morfeusz/wrappers/morfeuszpy3.i"
    DEPENDS libmorfeusz
)
add_custom_target (generate_python3_wrapper ALL
        DEPENDS "${SWIG_PYTHON_OUTFILE_CXX}" "${SWIG_PYTHON_OUTFILE_PY}")

set (SETUP_PY_IN "${CMAKE_CURRENT_SOURCE_DIR}/setup.py.in")
set (SETUP_PY_TMP    "${CMAKE_CURRENT_BINARY_DIR}/setupTmp.py")
set (SETUP_PY    "${CMAKE_CURRENT_BINARY_DIR}/setup.py")
set (DEPS        "${SWIG_PYTHON_OUTFILE_CXX}" "${SWIG_PYTHON_OUTFILE_PY}")
set (OUTPUT      "${CMAKE_CURRENT_BINARY_DIR}/build")

configure_file (${SETUP_PY_IN} ${SETUP_PY})

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/morfeusz2-0.4.0-py3.6" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

add_custom_target (generate_python3_setup_file
        DEPENDS ${SETUP_PY})

add_custom_command (OUTPUT ${OUTPUT}
                       COMMAND python3
                       ARGS ${SETUP_PY} build
                       DEPENDS generate_python3_wrapper generate_python3_setup_file libmorfeusz)

add_custom_target (build_python3_wrapper
        DEPENDS ${OUTPUT})

add_custom_target (py3morfeusz
        DEPENDS generate_python3_wrapper generate_python3_setup_file libmorfeusz build_python3_wrapper)

if (UNIX)
    add_custom_target (install-python3
        COMMAND python3 ${SETUP_PY} install --home=${CMAKE_INSTALL_PREFIX}
        DEPENDS py3morfeusz
    )
    add_custom_target (package-python3-bin
        COMMAND python3 ${SETUP_PY} bdist_egg -d "${TARGET_DIR}" --plat-name "${CMAKE_SYSTEM_NAME}-${ARCHITECTURE}"
        DEPENDS py3morfeusz
    )
    set (PACKAGE_DEPENDS "package-python3-bin")
    
    if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        add_custom_target (package-python3-deb-sdist
            COMMAND python3 ${SETUP_PY} --command-packages=stdeb.command sdist_dsc --depends "${CPACK_DEBIAN_PACKAGE_NAME}" --copyright-file "${PROJECT_SOURCE_DIR}/License.txt"
            DEPENDS py3morfeusz
        )
        add_custom_target (package-python3-deb-build
            COMMAND debuild -us -uc
            WORKING_DIRECTORY deb_dist/morfeusz2-${PY3MORFEUSZ_VERSION}
            DEPENDS package-python3-deb-sdist
        )
        add_custom_target(package-python3-deb
            COMMAND mkdir -p "${TARGET_DIR}" && cp ${CMAKE_CURRENT_BINARY_DIR}/deb_dist/python3-morfeusz2*.deb "${TARGET_DIR}"
            DEPENDS package-python3-deb-build
        )
        list (APPEND PACKAGE_DEPENDS package-python3-deb)
    endif ()
    add_custom_target (package-python3
        DEPENDS py3morfeusz ${PACKAGE_DEPENDS})
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Windows" AND NOT CMAKE_CROSSCOMPILING)
    add_custom_target (package-python3-win-installer
        COMMAND python3 ${SETUP_PY} bdist_wininst -d ${TARGET_DIR} --plat-name "${CMAKE_SYSTEM_NAME}-${ARCHITECTURE}"
        DEPENDS py3morfeusz
    )
    list (APPEND PACKAGE_DEPENDS package-python3-win-installer)
    add_custom_target (package-python3
        DEPENDS py3morfeusz ${PACKAGE_DEPENDS})
endif ()
