cmake_minimum_required(VERSION 2.8)

project(yas CXX)


find_package(GTest REQUIRED)

SET(CMAKE_BUILD_TYPE "Debug")  

if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
  set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} -Wall -Wno-unknown-pragmas -Wno-sign-compare -Woverloaded-virtual -Wwrite-strings -Wno-unused")
  set(CMAKE_CXX_FLAGS_DEBUG   "-O0 -g")
  set(CMAKE_CXX_FLAGS_RELEASE "-O2")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
endif()

add_subdirectory(src)


