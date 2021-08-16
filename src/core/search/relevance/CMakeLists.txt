LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/common)
include_directories(${CMAKE_SOURCE_DIR}/src/core/index ${CMAKE_SOURCE_DIR}/src/core/search
                    ${CMAKE_SOURCE_DIR}/src/common )
SET (LIB_RELEVANCE_SRC bm25_relevance.cpp tfidf_relevance.cpp boolean_relevance.cpp 
                   bm25_scorer.cpp boolean_scorer.cpp tfidf_scorer.cpp)
add_library(relevance SHARED ${LIB_RELEVANCE_SRC})
target_link_libraries(relevance pthread)