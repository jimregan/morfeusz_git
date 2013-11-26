# Make sure the compiler can find include files from our Hello library. 
#include_directories (${Morfeusz_SOURCE_DIR}/FSALibrary) 

# Make sure the linker can find the Hello library once it is built. 
#link_directories (${Morfeusz_BINARY_DIR}/Hello) 
include_directories (${Morfeusz_SOURCE_DIR}/fsa)
# add_executable (morfeusz2_analyze main.cpp)
#add_executable (test_morph test_morph.cpp MorphDeserializer.cpp Tagset.cpp ../fsa/const.cpp MorphInterpretation.cpp)
add_executable (test_morfeusz test_morfeusz.cpp MorphDeserializer.cpp Tagset.cpp ../fsa/const.cpp MorphInterpretation.cpp Morfeusz.cpp charset/CharsetConverter.cpp charset/CaseConverter.cpp charset/caseconv.cpp FlexionGraph.cpp)
add_executable (test_simple test_simple.cpp MorphDeserializer.cpp Tagset.cpp ../fsa/const.cpp MorphInterpretation.cpp Morfeusz.cpp charset/CharsetConverter.cpp charset/CaseConverter.cpp charset/caseconv.cpp FlexionGraph.cpp)

# Link the executable to the Hello library. 
#target_link_libraries (morfeusz2_analyze morfeusz2)
#set_target_properties ( morfeusz2_analyze PROPERTIES COMPILE_FLAGS "-std=gnu++0x" )

#set_target_properties ( test_morph PROPERTIES COMPILE_FLAGS "-std=gnu++0x -Wall -O2" )
set_target_properties ( test_morfeusz PROPERTIES COMPILE_FLAGS "-std=gnu++0x -Wall -g" )
set_target_properties ( test_simple PROPERTIES COMPILE_FLAGS "-std=gnu++0x -Wall -O2" )