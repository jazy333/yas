link_directories(${CMAKE_SOURCE_DIR}/src/common ${CMAKE_SOURCE_DIR}/src/algorithm 
                    ${CMAKE_SOURCE_DIR}/src/index  ${CMAKE_SOURCE_DIR}/src/search/match
                    ${CMAKE_SOURCE_DIR}/src/search/relevance)

include_directories(${CMAKE_SOURCE_DIR}/src/core/index ${CMAKE_SOURCE_DIR}/src/core/algorithm
                    ${CMAKE_SOURCE_DIR}/src/core/search/match  ${CMAKE_SOURCE_DIR}/src/core/search/
                    ${CMAKE_SOURCE_DIR}/src/core/db  ${CMAKE_SOURCE_DIR}/src/core/search/relevance)

set(LIB_QUERY_SRC any_match_query.cpp aug_and_scorer.cpp 
                    boolean_expression.cpp 
                    boolean_query.cpp
                    const_scorer.cpp none_match_query.cpp 
                    term_matcher.cpp term_query.cpp
                    term_scorer.cpp)

add_library(query SHARED ${LIB_QUERY_SRC})
target_link_libraries(query pthread algorithm match relevance)
set_target_properties(query PROPERTIES INSTALL_RPATH ${CMAKE_SOURCE_DIR}/lib)
install(TARGETS query DESTINATION lib)