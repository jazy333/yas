link_directories(${CMAKE_SOURCE_DIR}/src/common ${CMAKE_SOURCE_DIR}/src/algorithm 
                ${CMAKE_SOURCE_DIR}/src/core/search ${CMAKE_SOURCE_DIR}/src/core/compress)

include_directories(${CMAKE_SOURCE_DIR}/src/ ${CMAKE_SOURCE_DIR}/src/core/algorithm 
                    ${CMAKE_SOURCE_DIR}/src/core/search ${CMAKE_SOURCE_DIR}/src/core/index 
                    ${CMAKE_SOURCE_DIR}/src/core/index/memory ${CMAKE_SOURCE_DIR}/src/core/db
                    ${CMAKE_SOURCE_DIR}/src/core/search/match ${CMAKE_SOURCE_DIR}/src/core/tokenizer
                    ${CMAKE_SOURCE_DIR}/src/core/document ${CMAKE_SOURCE_DIR}/src/core/compress
                    ${CMAKE_SOURCE_DIR}/src/common   ${CMAKE_SOURCE_DIR}/src/core/search/query
                    ${CMAKE_SOURCE_DIR}/src/core/search/relevance ${CMAKE_SOURCE_DIR}/src/core/search/match)

set(LIB_SERIALIZED_INDEX_SRC invert_fields_index_writer.cpp block_term_reader.cpp
                            roaring_posting_list.cpp numeric_field_index_writer.cpp
                            numeric_field_index_reader.cpp binary_field_index_writer.cpp
                            binary_field_index_reader.cpp serialized_field_values_index_reader.cpp
                            serialized_invert_fields_index_reader.cpp segment_index_reader.cpp
                            serialized_point_fields_index_reader.cpp)

add_library(serialized_index SHARED ${LIB_SERIALIZED_INDEX_SRC})

target_link_libraries(serialized_index common pthread algorithm 
                        hash_db compression tokenizer document
                        memory_index match)
                        
set_target_properties(serialized_index PROPERTIES INSTALL_RPATH ${CMAKE_SOURCE_DIR}/lib)
SET_TARGET_PROPERTIES(serialized_index PROPERTIES VERSION 1.0.0.0 SOVERSION 1)

install(TARGETS serialized_index DESTINATION lib)

#unit test
set (TEST_SERIALIZED_SRC invert_fields_index_writer_test.cpp 
                        serialized_invert_fields_index_reader_test.cpp
                        roaring_posting_list_test.cpp
                        numeric_field_index_writer_test.cpp binary_field_index_writer_test.cpp
                        serialized_field_values_index_reader_test.cpp
                        point_field_index_writer_test.cpp
                        serialized_point_fields_index_reader_test.cpp)

add_executable(serialized_index_test ${TEST_SERIALIZED_SRC}) 
target_link_libraries(serialized_index_test gtest gtest_main serialized_index document db)