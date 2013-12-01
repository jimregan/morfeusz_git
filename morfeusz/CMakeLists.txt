
# Make sure the linker can find the Hello library once it is built. 
#link_directories (${Morfeusz_BINARY_DIR}/Hello)

include_directories (.)

add_library (libmorfeusz SHARED data/default_fsa.cpp MorphDeserializer.cpp Tagset.cpp fsa/const.cpp MorphInterpretation.cpp Morfeusz.cpp charset/CharsetConverter.cpp charset/CaseConverter.cpp charset/caseconv.cpp FlexionGraph.cpp)
set_target_properties(libmorfeusz PROPERTIES OUTPUT_NAME "morfeusz")

#add_executable (test_morfeusz test_morfeusz.cpp)
add_executable (morfeusz main.cpp)

target_link_libraries (morfeusz libmorfeusz)
#target_link_libraries (test_morfeusz libmorfeusz)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  target_link_libraries (libmorfeusz ws2_32)

  # here is the magic - must statically link libstdc++ and libgcc
  # but exclude libgcc_eh.a as it causes multiple definition of (...) errors

  set (CMAKE_SHARED_LINKER_FLAGS "-static-libstdc++ -static-libgcc -Wl,--exclude-libs,libgcc_eh.a")
  set (CMAKE_EXE_LINKER_FLAGS "-static-libstdc++ -static-libgcc")
endif (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
