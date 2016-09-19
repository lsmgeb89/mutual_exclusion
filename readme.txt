Compile Steps by CMake (https://cmake.org/):
1. create a build folder outside the src folder
   (eg: mkdir build_minsizerel)
2. change directory to the build folder
   (eg: cd build_minsizerel)
3. cmake 'path_to_source_root' -DCMAKE_BUILD_TYPE=MINSIZEREL
   (eg: cmake ../src -DCMAKE_BUILD_TYPE=MINSIZEREL)
4. make

How to run:
'path_to_lock_compare' 'thread_num' 'loop_num' 'test_times'
(eg: ./lock_compare 16 1000000 10)
