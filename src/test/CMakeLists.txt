LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/src/core/db/ ${CMAKE_CURRENT_SOURCE_DIR}/src/core/algorithm  ${CMAKE_CURRENT_SOURCE_DIR}/src/core/compression)
include_directories(${CMAKE_SOURCE_DIR}/src/core/db/ 
                    ${CMAKE_SOURCE_DIR}/src 
                    ${CMAKE_SOURCE_DIR}/src/core/algorithm 
                    ${CMAKE_SOURCE_DIR}/src/common
                    ${CMAKE_SOURCE_DIR}/src/core/compress
                    ${CMAKE_SOURCE_DIR}/src/core/log
                    ${CMAKE_SOURCE_DIR}/src/core/index
                    )
add_executable(test test.cpp)
target_link_libraries(test hash_db algorithm compression log)