# Make sure the compiler can find include files from our Hello library. 
#include_directories (${Morfeusz_SOURCE_DIR}/FSALibrary) 

# Make sure the linker can find the Hello library once it is built. 
#link_directories (${Morfeusz_BINARY_DIR}/Hello) 
include_directories (${Morfeusz_SOURCE_DIR}/fsa) 
add_library (morfeusz2 morfeusz.hpp morfeusz.cpp)
add_executable (morfeusz2_analyze main.cpp)

# Link the executable to the Hello library. 
target_link_libraries (morfeusz2_analyze morfeusz2)
set_target_properties ( morfeusz2_analyze PROPERTIES COMPILE_FLAGS "-std=gnu++0x" )
