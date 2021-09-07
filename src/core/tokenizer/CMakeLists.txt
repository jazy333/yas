#LINK_DIRECTORIES()
include_directories(${CMAKE_SOURCE_DIR}/src/)
link_directories(${CMAKE_SOURCE_DIR}/src/common)
set(LIB_TOKENIZER_SRC term.cpp simple_tokenizer.cpp delimeter_tokenizer.cpp)
add_library(tokenizer SHARED ${LIB_TOKENIZER_SRC})

target_link_libraries(tokenizer common)

#unit test
set(TEST_TOKENIZER_SRC term_test.cpp simple_tokenizer_test.cpp delimeter_tokenizer_test.cpp)
add_executable(tokenizer_test ${TEST_TOKENIZER_SRC}) 
target_link_libraries(tokenizer_test gtest gtest_main tokenizer common)

install(TARGETS tokenizer DESTINATION lib)
