
cmake_minimum_required (VERSION 2.8) 
project (Morfeusz) 
set(CMAKE_BUILD_TYPE "Debug")
enable_testing()

add_subdirectory (fsa)
add_subdirectory (morfeusz)

file(COPY fsabuilder testfiles DESTINATION .)

macro (test_build_and_recognize fname method)
    add_test (TestBuild-${method}-${fname} python fsabuilder/fsa/buildfsa.py -i testfiles/${fname} -o /tmp/test-${method}-${fname}.fsa --tagset-file=testfiles/polimorf.tagset --output-format=BINARY --serialization-method=${method})
    add_test (TestRecognize-${method}-${fname} morfeusz/test_morph /tmp/test-${method}-${fname}.fsa testfiles/${fname})
    # add_test (TestNOTRecognize-${method}-${fname} fsa/test_not_recognize /tmp/test-${method}-${fname}.fsa testfiles/out_of_dict)
    # add_test (TestSpeed-${method}-${fname} fsa/test_speed /tmp/test-${method}-${fname}.fsa testfiles/speed_test_data)
endmacro (test_build_and_recognize)

test_build_and_recognize(PoliMorfSmall.tab SIMPLE)
test_build_and_recognize(PoliMorfSmall.tab V1)
test_build_and_recognize(PoliMorfSmall.tab V2)

#test_build_and_recognize(dict_medium SIMPLE)
#test_build_and_recognize(dict_medium V1)
#test_build_and_recognize(dict_medium V2)

#test_build_and_recognize(dict_full SIMPLE)
#test_build_and_recognize(dict_full V1)
#test_build_and_recognize(dict_full V2)
