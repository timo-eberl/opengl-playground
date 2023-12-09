cmake_minimum_required(VERSION 3.5)
project(stb_image)

add_library(${PROJECT_NAME} include/stb_image.cpp)
# CMake is not able to determine the language, so we tell it
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE C)
target_include_directories(${PROJECT_NAME} PUBLIC include)
