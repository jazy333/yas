link_directories(${CMAKE_SOURCE_DIR}/src/common ${CMAKE_SOURCE_DIR}/src/algorithm 
                ${CMAKE_SOURCE_DIR}/src/core/search)

include_directories(${CMAKE_SOURCE_DIR}/src/core/index ${CMAKE_SOURCE_DIR}/src/core/algorithm 
                    ${CMAKE_SOURCE_DIR}/src/core/search ${CMAKE_SOURCE_DIR}/src/core/db
                    ${CMAKE_SOURCE_DIR}/src/core/search/match ${CMAKE_SOURCE_DIR}/src/core/document
                    ${CMAKE_SOURCE_DIR}/src/core/tokenizer)

set(LIB_MEMORY_INDEX_SRC memory_field_values_index_reader_wrapper.cpp 
                            memory_point_values_index_reader_wrapper.cpp
                            memory_index_reader.cpp memory_term_reader.cpp)

add_library(memory_index SHARED ${LIB_MEMORY_INDEX_SRC})

target_link_libraries(memory_index common pthread algorithm)
set_target_properties(memory_index PROPERTIES INSTALL_RPATH ${CMAKE_SOURCE_DIR}/lib)

install(TARGETS memory_index DESTINATION lib)