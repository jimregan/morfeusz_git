
SET(CMAKE_SYSTEM_NAME Darwin)
set (CMAKE_SYSTEM_VERSION 1)
set (CMAKE_C_COMPILER   /usr/x86_64-apple-darwin9/bin/x86_64-apple-darwin9-gcc)
set (CMAKE_CXX_COMPILER   /usr/x86_64-apple-darwin9/bin/x86_64-apple-darwin9-g++)

# here is the target environment located
set (CMAKE_FIND_ROOT_PATH  /usr/x86_64-apple-darwin9 /mnt/storage/JavaVM.framework)

# adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search 
# programs in the host environment
set (CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set (CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set (CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set (JAVA_JVM_LIBRARY /mnt/storage/JavaVM.framework)
set (JAVA_AWT_LIBRARY /mnt/storage/JavaVM.framework)
set (JAVA_INCLUDE_PATH /mnt/storage/JavaVM.framework/Headers)
