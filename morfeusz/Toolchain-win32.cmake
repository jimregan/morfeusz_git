
SET(CMAKE_SYSTEM_NAME Windows)
SET(CMAKE_SYSTEM_VERSION 1)
SET(CMAKE_C_COMPILER   i686-w64-mingw32-gcc)
SET(CMAKE_CXX_COMPILER i686-w64-mingw32-g++)
SET(CMAKE_RC_COMPILER i686-w64-mingw32-windres)

# here is the target environment located
set (WIN32_ROOT /mnt/storage/crossmorfeusz/windows32)
set (PYTHON_ROOT ${WIN32_ROOT}/Python27)
set (JAVA_ROOT ${WIN32_ROOT}/Java/jdk1.7.0_45)
set (CMAKE_FIND_ROOT_PATH  /usr/x86_64-w64-mingw32 ${JAVA_ROOT} ${PYTHON_ROOT})
set (CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${PYTHON_ROOT}/libs)

# adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search 
# programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)