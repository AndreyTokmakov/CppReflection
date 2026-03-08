cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
set(CMAKE_CXX_STANDARD 26)

project(reflection)
set(PROJECT_VERSION 1.0.0.0)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)

add_compile_options(-c -Wall -O3 -std=c++26)

add_executable(${PROJECT_NAME}
    main.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC ${UTILS_LIBRARY_DIR})
target_link_directories(${PROJECT_NAME} PUBLIC ${UTILS_BINARY_DIR})

TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    utils
    pthread
    ${EXTRA_LIBS}
)