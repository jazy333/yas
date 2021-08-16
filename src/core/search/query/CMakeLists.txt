LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/common ${CMAKE_SOURCE_DIR}/src/algorithm 
                    ${CMAKE_SOURCE_DIR}/src/index  ${CMAKE_SOURCE_DIR}/src/search/match
                    ${CMAKE_SOURCE_DIR}/src/search/relevance)
include_directories(${CMAKE_SOURCE_DIR}/src/core/index ${CMAKE_SOURCE_DIR}/src/core/algorithm)
SET (LIB_QUERY_SRC any_match_query.cpp aug_and_scorer.cpp boolean_expression.cpp boolean_query.cpp
                    const_scorer.cpp none_match_query.cpp term_matcher.cpp term_query.cpp
                    term_scorer.cpp)
add_library(query SHARED ${LIB_QUERY_SRC})
target_link_libraries(query pthread algorithm match)