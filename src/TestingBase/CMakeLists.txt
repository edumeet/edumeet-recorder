set(PROJECT_NAME TestingBase)
project(${PROJECT_NAME})
find_package(PkgConfig REQUIRED)
pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0)
include_directories(SYSTEM ${GSTREAMER_INCLUDE_DIRS})
include_directories("${CMAKE_SOURCE_DIR}/src/Common")
find_package(Boost 1.71 COMPONENTS system filesystem REQUIRED)

set(SRC
        main.cpp
        )

add_library(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} ${GSTREAMER_LIBRARIES} ConfigParser Boost::system Boost::filesystem dl)
file(COPY "test_config.json" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")