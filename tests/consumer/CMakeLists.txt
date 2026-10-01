cmake_minimum_required(VERSION 3.16)

project(cnumkit_external_consumer LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# This project intentionally depends only on the installed package.
find_package(cnumkit 0.1 CONFIG REQUIRED)

add_executable(cnumkit_external_consumer main.c)
target_link_libraries(cnumkit_external_consumer PRIVATE cnumkit::cnumkit)
