#LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/common ${CMAKE_SOURCE_DIR}/src/algorithm CMAKE_SOURCE_DIR}/src/core/search)
include_directories(${CMAKE_SOURCE_DIR}/src/ ${CMAKE_SOURCE_DIR}/src/core/algorithm {CMAKE_SOURCE_DIR}/src/core/search)
SET (LIB_DOCUMENT_SRC document.cpp text_field.cpp)
add_library(document SHARED ${LIB_DOCUMENT_SRC})
set_target_properties(document PROPERTIES INSTALL_RPATH ${CMAKE_SOURCE_DIR}/lib)
SET_TARGET_PROPERTIES(document PROPERTIES VERSION 1.0.0.0 SOVERSION 1)


#target_link_libraries()
SET (TEST_DOCUMENT_SRC document.cpp document_test.cpp)
add_executable(document_test ${TEST_DOCUMENT_SRC}) 
target_link_libraries(document_test gtest gtest_main)

install(TARGETS document DESTINATION lib)