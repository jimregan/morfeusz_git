
# SWIG
#set(CMAKE_SWIG_OUTDIR swig)
#~ FIND_PACKAGE (SWIG REQUIRED)
if (NOT PYTHON_INCLUDE_DIRS)
  set(Python_ADDITIONAL_VERSIONS 3.4)
  FIND_PACKAGE (PythonLibs ${PY} REQUIRED)
endif ()
#~ INCLUDE (${SWIG_USE_FILE})

set (PY3MORFEUSZ_VERSION "${Morfeusz_LIB_VERSION}")
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
  DEPENDS "${SWIG_PYTHON_OUTFILE_CXX}" "${SWIG_PYTHON_OUTFILE_PY}"
  )

set (SETUP_PY_IN "${CMAKE_CURRENT_SOURCE_DIR}/setup.py.in")
set (SETUP_PY_TMP    "${CMAKE_CURRENT_BINARY_DIR}/setupTmp.py")
set (SETUP_PY    "${CMAKE_CURRENT_BINARY_DIR}/setup.py")
set (DEPS        "${SWIG_PYTHON_OUTFILE_CXX}" "${SWIG_PYTHON_OUTFILE_PY}")
set (OUTPUT      "${CMAKE_CURRENT_BINARY_DIR}/build")

configure_file (${SETUP_PY_IN} ${SETUP_PY})

set (EGG_NAME "morfeusz2-${PY3MORFEUSZ_VERSION}-py${PY}")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${EGG_NAME}")
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/EGG-INFO" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/${EGG_NAME}/")

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
  add_custom_target (package-python3-egg
    COMMAND python3 ${SETUP_PY} bdist_egg -d "${TARGET_DIR}" --plat-name "${CMAKE_SYSTEM_NAME}-${ARCHITECTURE}"
    DEPENDS py3morfeusz
    )
  add_custom_target (package-python3-whl
    COMMAND python3 ${SETUP_PY} bdist_wheel -d "${TARGET_DIR}"
    #--plat-name "${CMAKE_SYSTEM_NAME}-${ARCHITECTURE}"
    DEPENDS py3morfeusz
    )
  configure_file (WHEEL.linux.in WHEEL)
  add_custom_target (package-python3-fatwhl
    COMMAND python3 setup.py build_py
    COMMAND python3 setup.py build_ext --rpath '\$\${ORIGIN}'
    COMMAND python3 setup.py bdist_wheel --skip-build --keep-temp --bdist-dir tmp.wheel
    COMMAND cp WHEEL tmp.wheel/morfeusz2-${PY3MORFEUSZ_VERSION}.dist-info/
    COMMAND cp ../../libmorfeusz2.so tmp.wheel/
    COMMAND python3 -m wheel pack --build-number ${DICT_VERSION} --dest-dir ${TARGET_DIR} tmp.wheel/
    COMMAND rm -r tmp.wheel
    DEPENDS generate_python3_wrapper generate_python3_setup_file libmorfeusz
    )
#  set (PACKAGE_DEPENDS package-python3-egg package-python3-whl)
  set (PACKAGE_DEPENDS package-python3-fatwhl)
  
  if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    if (BUILT_ON)
      add_custom_target (package-python3-deb-sdist
        COMMAND python3 ${SETUP_PY} --command-packages=stdeb.command sdist_dsc --depends3 libmorfeusz2 --copyright-file "${PROJECT_SOURCE_DIR}/License.txt"  --debian-version ${BUILT_ON}
        DEPENDS py3morfeusz
        )
    else ()
      add_custom_target (package-python3-deb-sdist
        COMMAND python3 ${SETUP_PY} --command-packages=stdeb.command sdist_dsc --depends3 libmorfeusz2 --copyright-file "${PROJECT_SOURCE_DIR}/License.txt"
        DEPENDS py3morfeusz
        )
    endif ()
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
    DEPENDS ${PACKAGE_DEPENDS})
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Windows" AND CMAKE_CROSSCOMPILING)
  add_custom_target (package-python3-egg-info
    COMMAND python3 ${SETUP_PY} egg_info
    DEPENDS generate_python3_wrapper generate_python3_setup_file libmorfeusz
    )
  add_custom_target (package-python3-dist-info
    COMMAND python3 ${SETUP_PY} dist_info
    DEPENDS generate_python3_setup_file generate_python3_wrapper
    )
  add_custom_target (build_pyd
    COMMAND echo Building pyd using ${CMAKE_CXX_COMPILER} and ${PYTHON_LIBRARIES}
    COMMAND ${CMAKE_CXX_COMPILER} -pthread -static-libgcc -static-libstdc++ -std=c++98 -DNDEBUG -DMS_WIN64 -g -fwrapv -O2 -Wall -fno-strict-aliasing -Wdate-time -D_FORTIFY_SOURCE=2 -g -fno-stack-protector -Wformat -Werror=format-security -fPIC -I${PROJECT_SOURCE_DIR}/morfeusz -I${PYTHON_INCLUDE_DIRS} -c ${CMAKE_CURRENT_BINARY_DIR}/swigPYTHON.cpp -o ${CMAKE_CURRENT_BINARY_DIR}/swigPYTHON.o
    COMMAND ${CMAKE_CXX_COMPILER} -pthread -static-libgcc -static-libstdc++ -DMS_WIN64 -shared -Wl,-O1 -Wl,-Bsymbolic-functions -fno-strict-aliasing -DNDEBUG -g -fwrapv -O2 -Wall -Wdate-time -D_FORTIFY_SOURCE=2 -g -fno-stack-protector -Wformat -Werror=format-security ${CMAKE_CURRENT_BINARY_DIR}/swigPYTHON.o -L${CMAKE_CURRENT_BINARY_DIR}/../.. -L${PYTHON_LIBRARIES} -lmorfeusz2 -lpython${PY_SHORT} -o ${CMAKE_CURRENT_BINARY_DIR}/_morfeusz2.pyd
    DEPENDS generate_python3_setup_file generate_python3_wrapper libmorfeusz
    )
  configure_file (WHEEL.win.in WHEEL)
  add_custom_target (package-python3-fatwhl
    COMMAND mkdir win-wheel
    COMMAND mv morfeusz2.dist-info win-wheel/morfeusz2-${PY3MORFEUSZ_VERSION}.dist-info
    COMMAND cp morfeusz2.py _morfeusz2.pyd ../../morfeusz2.dll win-wheel
    COMMAND cp WHEEL win-wheel/morfeusz2-${PY3MORFEUSZ_VERSION}.dist-info/
    COMMAND python3 -m wheel pack --build-number ${DICT_VERSION} --dest-dir ${TARGET_DIR} win-wheel/
    COMMAND rm -r win-wheel
    DEPENDS package-python3-dist-info build_pyd
    )
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin" AND CMAKE_CROSSCOMPILING)
  add_custom_target (package-python3-egg-info
    COMMAND python3 ${SETUP_PY} egg_info
    DEPENDS generate_python3_wrapper generate_python3_setup_file libmorfeusz
    )
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Windows" AND NOT CMAKE_CROSSCOMPILING)
  add_custom_target (package-python3-win-installer
    COMMAND python3 ${SETUP_PY} bdist_wininst -d ${TARGET_DIR} --plat-name "${CMAKE_SYSTEM_NAME}-${ARCHITECTURE}"
    DEPENDS py3morfeusz
    )
  list (APPEND PACKAGE_DEPENDS package-python3-win-installer)
  add_custom_target (package-python3
    DEPENDS py3morfeusz ${PACKAGE_DEPENDS})
endif ()
