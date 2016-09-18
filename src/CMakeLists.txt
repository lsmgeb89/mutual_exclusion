cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

project(lock_compare VERSION 1.0 LANGUAGES CXX C)

set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)

add_executable(lock_compare
  main.cc
  utils/tas_lock.h
  utils/ttas_lock.h
  utils/tournament_lock.h
  utils/peterson_lock.h
  utils/tester.h)

target_include_directories(lock_compare PRIVATE utils)

if(CMAKE_COMPILER_IS_GNUCXX)
  target_compile_options(lock_compare PRIVATE -std=c++11)
  target_link_libraries(lock_compare PRIVATE Threads::Threads)
endif()
