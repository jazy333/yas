SET (LIB_COMPRESS_SRC elias_fano_compression.cpp simd_bitpacking.cpp 
                        simd_binary_compression.cpp  bitpacking_compression.cpp)
add_library(compression SHARED ${LIB_COMPRESS_SRC})
set_target_properties(compression PROPERTIES INSTALL_RPATH ${CMAKE_SOURCE_DIR}/lib)
SET_TARGET_PROPERTIES(compression PROPERTIES VERSION 1.0.0.0 SOVERSION 1)
LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/common)
include_directories(${CMAKE_SOURCE_DIR}/src/common)
SET (TEST_COMPRESS_SRC  bitpacking_compression_test.cpp)
add_executable(compress_test ${TEST_COMPRESS_SRC}) 
target_link_libraries(compress_test gtest gtest_main compression)

install(TARGETS compression DESTINATION lib)