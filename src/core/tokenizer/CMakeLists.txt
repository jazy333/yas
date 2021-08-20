#LINK_DIRECTORIES()
SET (LIB_TOKENIZER_SRC term.cpp simple_tokenizer.cpp)
add_library(tokenizer SHARED ${LIB_TOKENIZER_SRC})

#unit test
SET (TEST_TOKENIZER_SRC term_test.cpp simple_tokenizer_test.cpp)
add_executable(tokenizer_test ${TEST_TOKENIZER_SRC}) 
target_link_libraries(tokenizer_test gtest gtest_main tokenizer)
