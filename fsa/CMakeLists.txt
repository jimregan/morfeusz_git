
add_executable (test_dict test_dict.cpp)
add_executable (test_speed test_speed.cpp)
add_executable (test_speed_profile test_speed.cpp)
set_target_properties ( test_dict PROPERTIES COMPILE_FLAGS "-std=gnu++0x -Wall -g" )
set_target_properties ( test_speed PROPERTIES COMPILE_FLAGS "-std=gnu++0x -Wall -O2" )
set_target_properties ( test_speed_profile PROPERTIES COMPILE_FLAGS "-std=gnu++0x -Wall -g -O2" )
