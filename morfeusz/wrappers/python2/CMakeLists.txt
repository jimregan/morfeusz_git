
# SWIG
#set(CMAKE_SWIG_OUTDIR swig)
#~ FIND_PACKAGE (SWIG REQUIRED)
FIND_PACKAGE (PythonLibs ${PY} EXACT)
#~ INCLUDE (${SWIG_USE_FILE})

set (PY2MORFEUSZ_VERSION "${Morfeusz_LIB_VERSION}")

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

set (EGG_NAME "morfeusz2-${PY2MORFEUSZ_VERSION}-py${PY}")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${EGG_NAME}")
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/EGG-INFO" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/${EGG_NAME}/")

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
    add_custom_target (package-python2-egg
        COMMAND python2 ${SETUP_PY} bdist_egg -d "${TARGET_DIR}" --plat-name "${CMAKE_SYSTEM_NAME}-${ARCHITECTURE}"
        DEPENDS py2morfeusz
    )
    add_custom_target (package-python2-whl
        COMMAND python2 ${SETUP_PY} bdist_wheel -d "${TARGET_DIR}"
        DEPENDS py2morfeusz
    )
    set (PACKAGE_DEPENDS "package-python2-egg" "package-python2-whl")
    
    if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
      if (BUILT_ON)
        add_custom_target (package-python2-deb-sdist
            COMMAND python2 ${SETUP_PY} --command-packages=stdeb.command sdist_dsc --depends "${CPACK_DEBIAN_PACKAGE_NAME}" --copyright-file "${PROJECT_SOURCE_DIR}/License.txt" --debian-version ${BUILT_ON}
            DEPENDS py2morfeusz
        )
      else ()
        add_custom_target (package-python2-deb-sdist
            COMMAND python2 ${SETUP_PY} --command-packages=stdeb.command sdist_dsc --depends "${CPACK_DEBIAN_PACKAGE_NAME}" --copyright-file "${PROJECT_SOURCE_DIR}/License.txt"
            DEPENDS py2morfeusz
        )
      endif ()
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
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Windows" AND CMAKE_CROSSCOMPILING)
    add_custom_target (package-python2-egg-info
      COMMAND python ${SETUP_PY} egg_info
      DEPENDS generate_python2_wrapper generate_python2_setup_file libmorfeusz
    )
    add_custom_target (package-python2-dist-info
      COMMAND python2 ${SETUP_PY} dist_info
      DEPENDS generate_python2_setup_file generate_python2_wrapper
      )
    add_custom_target (build_pyd2
      COMMAND echo Building pyd2 using ${CMAKE_CXX_COMPILER}
      COMMAND ${CMAKE_CXX_COMPILER} -pthread -static-libgcc -static-libstdc++ -std=c++98 -DNDEBUG -DMS_WIN64 -g -fwrapv -O2 -Wall -fno-strict-aliasing -Wdate-time -D_FORTIFY_SOURCE=2 -g -fno-stack-protector -Wformat -Werror=format-security -fPIC -I${PROJECT_SOURCE_DIR}/morfeusz -I${PYTHON_INCLUDE_DIRS} -c ${CMAKE_CURRENT_BINARY_DIR}/swigPYTHON.cpp -o ${CMAKE_CURRENT_BINARY_DIR}/swigPYTHON.o
      COMMAND ${CMAKE_CXX_COMPILER} -pthread -static-libgcc -static-libstdc++ -DMS_WIN64 -shared -Wl,-O1 -Wl,-Bsymbolic-functions -fno-strict-aliasing -DNDEBUG -g -fwrapv -O2 -Wall -Wdate-time -D_FORTIFY_SOURCE=2 -g -fno-stack-protector -Wformat -Werror=format-security ${CMAKE_CURRENT_BINARY_DIR}/swigPYTHON.o -L${CMAKE_CURRENT_BINARY_DIR}/../.. -L${PYTHON_LIBRARIES} -lmorfeusz2 -lpython27 -o ${CMAKE_CURRENT_BINARY_DIR}/_morfeusz2.pyd
      DEPENDS generate_python2_setup_file generate_python2_wrapper libmorfeusz
      )
    configure_file (WHEEL.win.in WHEEL)
    add_custom_target (package-python2-fatwhl
      COMMAND mkdir win-wheel
      COMMAND mv morfeusz2.dist-info win-wheel/morfeusz2-${PY2MORFEUSZ_VERSION}.dist-info
      COMMAND cp morfeusz2.py _morfeusz2.pyd ../../morfeusz2.dll win-wheel
      COMMAND cp WHEEL win-wheel/morfeusz2-${PY2MORFEUSZ_VERSION}.dist-info/
      COMMAND python2 -m wheel pack --build-number ${DICT_VERSION} --dest-dir ${TARGET_DIR} win-wheel/
      COMMAND rm -r win-wheel
      DEPENDS package-python2-dist-info build_pyd2
      )
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin" AND CMAKE_CROSSCOMPILING)
    add_custom_target (package-python2-egg-info
      COMMAND python ${SETUP_PY} egg_info
      DEPENDS generate_python2_wrapper generate_python2_setup_file libmorfeusz
    )
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
