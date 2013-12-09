
SET(CMAKE_SYSTEM_NAME Darwin)
SET(CMAKE_SYSTEM_VERSION 1)
SET(CMAKE_C_COMPILER   /home/mlenart/usr/i686-apple-darwin11/bin/i686-apple-darwin11-gcc-4.6)
SET(CMAKE_CXX_COMPILER   /home/mlenart/usr/i686-apple-darwin11/bin/i686-apple-darwin11-g++-4.6)
#SET(CMAKE_RC_COMPILER x86_64-w64-mingw32-windres)

# here is the target environment located
SET(CMAKE_FIND_ROOT_PATH  /home/mlenart/usr/i686-apple-darwin11:/home/mlenart/usr/i686-apple-darwin11/lib/gcc/i686-apple-darwin11/4.6.4 )

# adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search 
# programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)