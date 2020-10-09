set(TEST_NAME test_Recorder)
include_directories(${CMAKE_ROOT})
find_package(PkgConfig REQUIRED)
pkg_check_modules(GSTREAMER_PB REQUIRED gstreamer-plugins-base-1.0 gstreamer-pbutils-1.0)
set(SRC
        test_rtpRecorder.cpp
        test_htmlRecorder.cpp
)

add_executable(${TEST_NAME}  ${SRC})
target_link_libraries(${TEST_NAME} TestingBase)
target_link_libraries(${TEST_NAME} Recorder)
target_link_libraries(${TEST_NAME} ${GSTREAMER_PB_LIBRARIES})
find_package(Catch2 REQUIRED)
include(CTest)
include(Catch)
catch_discover_tests(${TEST_NAME} WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
