LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/common ${CMAKE_SOURCE_DIR}/src/core/algorithm 
                    ${CMAKE_SOURCE_DIR}/src/core/index  ${CMAKE_SOURCE_DIR}/src/core/tokenizer)

include_directories(${CMAKE_SOURCE_DIR}/src/core/index ${CMAKE_SOURCE_DIR}/src/core/algorithm 
                    ${CMAKE_SOURCE_DIR}/src/core/tokenizer)
SET (LIB_SEARCH_SRC term_stat.cpp)
add_library(search SHARED ${LIB_SEARCH_SRC})
target_link_libraries(search pthread algorithm tokenizer)

add_subdirectory(match)
add_subdirectory(relevance)
add_subdirectory(query)