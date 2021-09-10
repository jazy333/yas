link_directories(${CMAKE_SOURCE_DIR}/src/common ${CMAKE_SOURCE_DIR}/src/algorithm 
                    ${CMAKE_SOURCE_DIR}/src/core/search ${CMAKE_SOURCE_DIR}/src/core/compress )

include_directories(${CMAKE_SOURCE_DIR}/src/ ${CMAKE_SOURCE_DIR}/src/core/algorithm 
                    ${CMAKE_SOURCE_DIR}/src/core/search ${CMAKE_SOURCE_DIR}/src/core/document
                    ${CMAKE_SOURCE_DIR}/src/core/db  ${CMAKE_SOURCE_DIR}/src/core/search/match
                    ${CMAKE_SOURCE_DIR}/src/core/tokenizer ${CMAKE_SOURCE_DIR}/src/core/log
                    ${CMAKE_SOURCE_DIR}/src/core/index/serialized ${CMAKE_SOURCE_DIR}/src/core/index
                    ${CMAKE_SOURCE_DIR}/src/common ${CMAKE_SOURCE_DIR}/src/core/index/memory
                    ${CMAKE_SOURCE_DIR}/src/core/search/query ${CMAKE_SOURCE_DIR}/src/core/search/relevance)

set(LIB_INDEX_SRC index_reader.cpp index_writer.cpp index_merger.cpp)
add_library(index SHARED ${LIB_INDEX_SRC})
target_link_libraries(index common pthread algorithm  document hash_db 
                        log serialized_index memory_index compression)
set_target_properties(index PROPERTIES INSTALL_RPATH ${CMAKE_SOURCE_DIR}/lib)

add_subdirectory(memory)
add_subdirectory(serialized)

#unit test
set (TEST_INDEX_SRC index_reader_test.cpp index_writer_test.cpp index_merger_test.cpp)

add_executable(index_test ${TEST_INDEX_SRC}) 
target_link_libraries(index_test gtest gtest_main serialized_index document db index)


install(TARGETS index DESTINATION lib)