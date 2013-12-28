
SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_SYSTEM_VERSION 1)
SET(CMAKE_C_COMPILER   gcc -m32)
SET(CMAKE_CXX_COMPILER g++ -m32)

set (JAVA_ROOT /mnt/storage/crossmorfeusz/linux32/jdk1.7.0_45)
set (PYTHON_ROOT /mnt/storage/crossmorfeusz/linux32/python)

# here is the target environment located
set (CMAKE_FIND_ROOT_PATH  ${JAVA_ROOT} ${PYTHON_ROOT})

# adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search 
# programs in the host environment
set (CMAKE_FIND_ROOT_PATH_MODE_PROGRAM BOTH)
set (CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)
set (CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)

set (JAVA_INCLUDE_PATH ${JAVA_ROOT}/include)
set (JAVA_INCLUDE_PATH2 ${JAVA_ROOT}/include/linux)