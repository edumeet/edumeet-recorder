set(PROJECT_NAME edumeet-recorder)
project(${PROJECT_NAME})

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
include_directories("${CMAKE_SOURCE_DIR}/src/Common")
find_package(Boost 1.71 COMPONENTS system filesystem REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0)
include_directories(${GSTREAMER_INCLUDE_DIRS})

set(SRC
        main.cpp
        )

add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} Common Boost::system Boost::filesystem dl)
file(COPY "config.json" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
