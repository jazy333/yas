SET (LIB_ALGORITHM_SRC murmur_hash2.cpp fnv_hash.cpp intro_sorter.cpp  sorter.cpp)
add_library(algorithm SHARED ${LIB_ALGORITHM_SRC})
set_target_properties(algorithm PROPERTIES INSTALL_RPATH ${CMAKE_SOURCE_DIR}/lib)
SET_TARGET_PROPERTIES(algorithm PROPERTIES VERSION 1.0.0.0 SOVERSION 1)
install(TARGETS algorithm DESTINATION lib)