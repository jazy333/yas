
LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/common)
include_directories(${CMAKE_SOURCE_DIR}/src/)
SET (LIB_HASH_DB_SRC hash_db.cpp mmap_file.cpp)
add_library(hash_db SHARED ${LIB_HASH_DB_SRC})
target_link_libraries(hash_db common pthread)