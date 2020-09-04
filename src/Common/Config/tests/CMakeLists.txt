set(TEST_NAME test_ConfigParser)

include_directories(${CMAKE_ROOT})

set(SRC
        test_config.cpp
        )

add_executable(${TEST_NAME} ${SRC})

target_link_libraries(${TEST_NAME} TestingBase)
find_package(Catch2 REQUIRED)
include(CTest)
include(Catch)
catch_discover_tests(${TEST_NAME} WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})