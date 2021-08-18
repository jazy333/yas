link_directories(${CMAKE_SOURCE_DIR}/src/common ${CMAKE_SOURCE_DIR}/src/algorithm 
                ${CMAKE_SOURCE_DIR}/src/core/search)

include_directories(${CMAKE_SOURCE_DIR}/src/ ${CMAKE_SOURCE_DIR}/src/core/algorithm 
                    ${CMAKE_SOURCE_DIR}/src/core/search ${CMAKE_SOURCE_DIR}/src/core/index 
                    ${CMAKE_SOURCE_DIR}/src/core/index/memory ${CMAKE_SOURCE_DIR}/src/core/db
                    ${CMAKE_SOURCE_DIR}/src/core/search/match ${CMAKE_SOURCE_DIR}/src/core/tokenizer
                    ${CMAKE_SOURCE_DIR}/src/core/document ${CMAKE_SOURCE_DIR}/src/core/compress
                    ${CMAKE_SOURCE_DIR}/src/common   ${CMAKE_SOURCE_DIR}/src/core/search/query
                    ${CMAKE_SOURCE_DIR}/src/core/search/relevance)

set(LIB_SERIALIZED_INDEX_SRC invert_fields_index_writer.cpp block_term_reader.cpp)

add_library(serialized_index SHARED ${LIB_SERIALIZED_INDEX_SRC})

target_link_libraries(serialized_index common pthread algorithm hash_db)