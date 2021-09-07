SET (LIB_ALGORITHM_SRC murmur_hash2.cpp fnv_hash.cpp intro_sorter.cpp  sorter.cpp)
add_library(algorithm SHARED ${LIB_ALGORITHM_SRC})

install(TARGETS algorithm DESTINATION lib)