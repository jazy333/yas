LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/common ${CMAKE_SOURCE_DIR}/src/core/search)
include_directories(${CMAKE_SOURCE_DIR}/src/core/index ${CMAKE_SOURCE_DIR}/src/core/search
                    ${CMAKE_SOURCE_DIR}/src/common )
SET (LIB_RELEVANCE_SRC bm25_relevance.cpp tfidf_relevance.cpp boolean_relevance.cpp 
                   bm25_scorer.cpp boolean_scorer.cpp tfidf_scorer.cpp)
add_library(relevance SHARED ${LIB_RELEVANCE_SRC})
set_target_properties(relevance PROPERTIES INSTALL_RPATH ${CMAKE_SOURCE_DIR}/lib)
SET_TARGET_PROPERTIES(relevance PROPERTIES VERSION 1.0.0.0 SOVERSION 1)
target_link_libraries(relevance pthread search common ) 

#unit test
SET (TEST_RELEVANCE_SRC boolean_scorer_test.cpp boolean_relevance_test.cpp)
add_executable(relevance_test ${TEST_RELEVANCE_SRC}) 
target_link_libraries(relevance_test gtest gtest_main search relevance )

install(TARGETS relevance DESTINATION lib)