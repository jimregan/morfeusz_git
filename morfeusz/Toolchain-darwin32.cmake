
SET(CMAKE_SYSTEM_NAME Darwin)
SET(CMAKE_SYSTEM_VERSION 9)
SET(CMAKE_C_COMPILER   /mnt/storage/usr/i686-apple-darwin9/i686-apple-darwin9-gcc)
SET(CMAKE_CXX_COMPILER   /mnt/storage/usr/i686-apple-darwin9/i686-apple-darwin9-g++)

# here is the target environment located
SET(CMAKE_FIND_ROOT_PATH  /mnt/storage/usr/i686-apple-darwin9)

# adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search 
# programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
