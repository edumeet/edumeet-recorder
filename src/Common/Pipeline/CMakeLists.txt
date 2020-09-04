set(PROJECT_NAME Pipeline)
project (${PROJECT_NAME})
include_directories(.)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0)
include_directories(${GSTREAMER_INCLUDE_DIRS})

find_package(fmt)

set(SRC
        Pipeline.h
        Pipeline.cpp
        )

add_library(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} ${GSTREAMER_LIBRARIES}  fmt::fmt)

if (BUILD_TESTS)
    add_subdirectory(tests)
endif (BUILD_TESTS)