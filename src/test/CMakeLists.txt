LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/src/core/db/ ${CMAKE_CURRENT_SOURCE_DIR}/src/core/algorithm)
include_directories(${CMAKE_SOURCE_DIR}/src/core/db/ ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/src/core/algorithm ${CMAKE_SOURCE_DIR}/src/common)
add_executable(test test.cpp)
target_link_libraries(test hash_db algorithm )