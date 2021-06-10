SET (LIB_COMPRESS_SRC elias_fano_compression.cpp simd_bitpacking.cpp simd_binary_compression.cpp)
add_library(compress SHARED ${LIB_COMPRESS_SRC})
LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/common)
include_directories(${CMAKE_SOURCE_DIR}/src/common)
