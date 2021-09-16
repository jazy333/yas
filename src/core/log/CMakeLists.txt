SET (LIB_LOG_SRC logger.cpp sink.cpp file_sink.cpp default_formatter.cpp log.cpp loggers.cpp)
add_library(log SHARED ${LIB_LOG_SRC})
LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/common)
include_directories(${CMAKE_SOURCE_DIR}/src/common)
#target_link_libraries(pthread)
SET_TARGET_PROPERTIES(log PROPERTIES VERSION 1.0.0.0 SOVERSION 1)

install(TARGETS log DESTINATION lib)