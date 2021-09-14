link_directories(${CMAKE_SOURCE_DIR}/src/common ${CMAKE_SOURCE_DIR}/src/core/algorithm 
                    ${CMAKE_SOURCE_DIR}/src/core/index  ${CMAKE_SOURCE_DIR}/src/core/tokenizer
                    ${CMAKE_SOURCE_DIR}/src/core/search/query)

include_directories(${CMAKE_SOURCE_DIR}/src/core/index ${CMAKE_SOURCE_DIR}/src/core/algorithm 
                    ${CMAKE_SOURCE_DIR}/src/core/tokenizer ${CMAKE_SOURCE_DIR}/src/core/document
                    ${CMAKE_SOURCE_DIR}/src/core/db ${CMAKE_SOURCE_DIR}/src/core/search/match
                    ${CMAKE_SOURCE_DIR}/src/core/search ${CMAKE_SOURCE_DIR}/src/core/search/query
                    ${CMAKE_SOURCE_DIR}/src/core/search/relevance ${CMAKE_SOURCE_DIR}/src/
                    ${CMAKE_SOURCE_DIR}/src/core/log)
                    
set(LIB_SEARCH_SRC term_stat.cpp index_searcher.cpp)
add_library(search SHARED ${LIB_SEARCH_SRC})
target_link_libraries(search pthread algorithm tokenizer index match common)
set_target_properties(search PROPERTIES INSTALL_RPATH ${CMAKE_SOURCE_DIR}/lib)

add_subdirectory(match)
add_subdirectory(relevance)
add_subdirectory(query)

#unit test
set (TEST_SEARCHER_SRC index_searcher_test.cpp)
add_executable(searcher_test ${TEST_SEARCHER_SRC}) 
target_link_libraries(searcher_test gtest gtest_main common serialized_index document db index search
                                    match relevance tokenizer query )
                                    

install(TARGETS search DESTINATION lib)