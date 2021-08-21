
link_directories(${CMAKE_SOURCE_DIR}/src/common ${CMAKE_SOURCE_DIR}/src/algorithm
                ${CMAKE_SOURCE_DIR}/src/core/log)
include_directories(${CMAKE_SOURCE_DIR}/src/ ${CMAKE_SOURCE_DIR}/src/core/algorithm
                    ${CMAKE_SOURCE_DIR}/src/core/log)
set(LIB_HASH_DB_SRC hash_db.cpp mmap_file.cpp memory_file.cpp file_slice.cpp)
add_library(hash_db SHARED ${LIB_HASH_DB_SRC})
target_link_libraries(hash_db common pthread algorithm log)