set(LIB_COMMON_SRC common.cpp shared_mutex.cpp sortable_bytes.cpp)
add_library(common SHARED ${LIB_COMMON_SRC})
SET_TARGET_PROPERTIES(common PROPERTIES VERSION 1.0.0.0 SOVERSION 1)
#target_link_libraries(common pthread)
set(TEST_COMMON_SRC fixed_bit_set_test.cpp skip_list_test.cpp  common.cpp common_test.cpp)
add_executable(common_test ${TEST_COMMON_SRC}) 
target_link_libraries(common_test gtest gtest_main)

install(TARGETS common DESTINATION lib)
