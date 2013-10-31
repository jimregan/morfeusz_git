
add_executable (test_speed test_speed.cpp)
add_executable (test_speed_profile test_speed.cpp)
add_executable (test_recognize test_recognize.cpp)
add_executable (test_not_recognize test_not_recognize.cpp)
set_target_properties ( test_speed PROPERTIES COMPILE_FLAGS "-std=gnu++0x -Wall -O2" )
set_target_properties ( test_speed_profile PROPERTIES COMPILE_FLAGS "-std=gnu++0x -Wall -O2 -g" )
set_target_properties ( test_recognize PROPERTIES COMPILE_FLAGS "-std=gnu++0x -Wall -O2" )
set_target_properties ( test_not_recognize PROPERTIES COMPILE_FLAGS "-std=gnu++0x -Wall -O2" )
