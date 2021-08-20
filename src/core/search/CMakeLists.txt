link_directories(${CMAKE_SOURCE_DIR}/src/common ${CMAKE_SOURCE_DIR}/src/core/algorithm 
                    ${CMAKE_SOURCE_DIR}/src/core/index  ${CMAKE_SOURCE_DIR}/src/core/tokenizer)

include_directories(${CMAKE_SOURCE_DIR}/src/core/index ${CMAKE_SOURCE_DIR}/src/core/algorithm 
                    ${CMAKE_SOURCE_DIR}/src/core/tokenizer ${CMAKE_SOURCE_DIR}/src/core/document
                    ${CMAKE_SOURCE_DIR}/src/core/db ${CMAKE_SOURCE_DIR}/src/core/search/match
                    ${CMAKE_SOURCE_DIR}/src/core/search ${CMAKE_SOURCE_DIR}/src/core/search/query
                    ${CMAKE_SOURCE_DIR}/src/core/search/relevance)
set(LIB_SEARCH_SRC term_stat.cpp index_searcher.cpp)
add_library(search SHARED ${LIB_SEARCH_SRC})
target_link_libraries(search pthread algorithm tokenizer index match)

add_subdirectory(match)
add_subdirectory(relevance)
add_subdirectory(query)