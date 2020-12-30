SET (LIB_COMMON_SRC common.cpp shared_mutex.cpp)
add_library(common SHARED ${LIB_COMMON_SRC})
target_link_libraries(pthread)