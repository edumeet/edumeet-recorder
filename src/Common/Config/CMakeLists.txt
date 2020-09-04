set(PROJECT_NAME ConfigParser)
project (${PROJECT_NAME})
include_directories(.)
set(SRC
        ConfigParser.h
        ConfigParser.cpp
        )

add_library(${PROJECT_NAME} ${SRC})

if (BUILD_TESTS)
    add_subdirectory(tests)
endif (BUILD_TESTS)