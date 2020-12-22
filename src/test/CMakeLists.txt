LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/src/core/db/)
include_directories(${CMAKE_SOURCE_DIR}/src/core/db/ ${CMAKE_SOURCE_DIR}/src)
add_executable(test test.cpp)
target_link_libraries(test hash_db)