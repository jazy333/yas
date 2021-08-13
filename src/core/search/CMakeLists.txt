LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/common ${CMAKE_SOURCE_DIR}/src/algorithm ${CMAKE_SOURCE_DIR}/src/index)
include_directories(${CMAKE_SOURCE_DIR}/src/core/index ${CMAKE_SOURCE_DIR}/src/core/algorithm)
#SET (LIB_SEARCH_SRC index_searcher.cpp and_posting_list.cpp or_posting_list.cpp)
#add_library(search SHARED ${LIB_SEARCH_SRC})
#target_link_libraries(search pthread algorithm)

add_subdirectory(match)
#add_subdirectory(relevance)
#add_subdirectory(query)