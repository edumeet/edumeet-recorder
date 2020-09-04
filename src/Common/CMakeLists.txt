set(PROJECT_NAME Common)
project(${PROJECT_NAME})

include_directories("${CMAKE_SOURCE_DIR}/src/Common")

add_subdirectory(Rest)
add_subdirectory(Recorder)
add_subdirectory(Pipeline)
add_subdirectory(Config)

add_library(${PROJECT_NAME} INTERFACE)
# link libraries
target_link_libraries(${PROJECT_NAME} INTERFACE Rest Recorder ConfigParser)
