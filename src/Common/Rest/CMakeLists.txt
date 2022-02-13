set(PROJECT_NAME Rest)
project (${PROJECT_NAME})
include_directories(.)
find_package(cpprestsdk REQUIRED)
set(SRC
        Service.h
        Service.cpp
        )

add_library(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} cpprestsdk::cpprest -lssl -lcrypto)
