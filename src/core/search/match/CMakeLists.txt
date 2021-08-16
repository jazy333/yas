LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/core/search/query ${CMAKE_SOURCE_DIR}/src/common)
include_directories(${CMAKE_SOURCE_DIR}/src/core/search/query ${CMAKE_SOURCE_DIR}/src/common)
SET (LIB_MATCH_SRC  and_posting_list.cpp sequence_posting_list.cpp any_posting_list.cpp
                     none_posting_list.cpp not_posting_list.cpp or_posting_list.cpp
                     bit_set_posting_list.cpp weak_and_posting_list.cpp)
add_library(match SHARED ${LIB_MATCH_SRC})
target_link_libraries(match common)
#unit test
SET (TEST_MATCH_SRC and_posting_list_test.cpp sequence_posting_list_test.cpp 
                        any_posting_list_test.cpp none_posting_list_test.cpp)
add_executable(match_test ${TEST_MATCH_SRC}) 
target_link_libraries(match_test gtest gtest_main match)