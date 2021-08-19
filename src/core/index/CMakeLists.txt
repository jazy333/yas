link_directories(${CMAKE_SOURCE_DIR}/src/common ${CMAKE_SOURCE_DIR}/src/algorithm 
                    ${CMAKE_SOURCE_DIR}/src/core/search)

include_directories(${CMAKE_SOURCE_DIR}/src/ ${CMAKE_SOURCE_DIR}/src/core/algorithm 
                    ${CMAKE_SOURCE_DIR}/src/core/search ${CMAKE_SOURCE_DIR}/src/core/document
                    ${CMAKE_SOURCE_DIR}/src/core/db  ${CMAKE_SOURCE_DIR}/src/core/search/match
                    ${CMAKE_SOURCE_DIR}/src/core/tokenizer ${CMAKE_SOURCE_DIR}/src/core/log
                    ${CMAKE_SOURCE_DIR}/src/core/index/serialized ${CMAKE_SOURCE_DIR}/src/core/index
                    ${CMAKE_SOURCE_DIR}/src/common ${CMAKE_SOURCE_DIR}/src/core/index/memory)

set(LIB_INDEX_SRC index_reader.cpp index_writer.cpp)
add_library(index SHARED ${LIB_INDEX_SRC})
target_link_libraries(index common pthread algorithm serialized_index)

add_subdirectory(memory)
add_subdirectory(serialized)