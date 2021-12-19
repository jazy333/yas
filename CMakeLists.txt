cmake_minimum_required(VERSION 2.8)

project(yas CXX)

SET(CMAKE_INSTALL_PREFIX  ${CMAKE_SOURCE_DIR})
find_package(GTest REQUIRED)

SET(CMAKE_BUILD_TYPE "Debug")  
#SET(CMAKE_BUILD_TYPE "Release")  

if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
  set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} -Wall -Wno-unknown-pragmas -Wno-sign-compare -Woverloaded-virtual -Wwrite-strings -Wno-unused")
  set(CMAKE_CXX_FLAGS_DEBUG   "-O0 -g")
  set(CMAKE_CXX_FLAGS_RELEASE "-O2")
  #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
endif()

add_subdirectory(src)

install(DIRECTORY "${CMAKE_SOURCE_DIR}/src/core" # source directory
        DESTINATION "include" # target directory
        FILES_MATCHING # install only matched files
        PATTERN "*.h" # select header files
        PATTERN "CMakeFiles" EXCLUDE
        PATTERN ".git" EXCLUDE 
)

install(DIRECTORY "${CMAKE_SOURCE_DIR}/src/common" # source directory
        DESTINATION "include" # target directory
        FILES_MATCHING # install only matched files
        PATTERN "*.h" # select header files
        PATTERN "CMakeFiles" EXCLUDE
        PATTERN ".git" EXCLUDE 
)

#add_custom_command(TARGET all POST_BUILD COMMAND cp ./lib/* ../_lib)
add_custom_target(check)
