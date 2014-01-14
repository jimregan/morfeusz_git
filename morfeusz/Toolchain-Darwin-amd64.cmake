
set (DARWIN64_ROOT /mnt/storage/crossmorfeusz/darwin64)

set (CMAKE_SYSTEM_NAME Darwin)
set (CMAKE_SYSTEM_VERSION 1)
set (CMAKE_C_COMPILER   ${DARWIN64_ROOT}/x86_64-apple-darwin9/bin/x86_64-apple-darwin9-gcc)
set (CMAKE_CXX_COMPILER   ${DARWIN64_ROOT}/x86_64-apple-darwin9/bin/x86_64-apple-darwin9-g++)

set (CMAKE_SYSTEM_PROCESSOR "x86_64")

set (JAVA_ROOT ${DARWIN64_ROOT}/JavaVM.framework)
set (PYTHON_ROOT ${DARWIN64_ROOT}/Python.framework/Versions/2.7)

# here is the target environment located
set (CMAKE_FIND_ROOT_PATH ${DARWIN64_ROOT}/x86_64-apple-darwin9 ${JAVA_ROOT} ${PYTHON_ROOT})

# adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search 
# programs in the host environment
set (CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set (CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set (CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set (JAVA_JVM_LIBRARY ${JAVA_ROOT})
set (JAVA_AWT_LIBRARY ${JAVA_ROOT})
set (JAVA_INCLUDE_PATH ${JAVA_ROOT}/Headers)
