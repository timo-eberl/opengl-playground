cmake_minimum_required(VERSION 3.5)
project(mikktspace)

add_library(${PROJECT_NAME} src/mikktspace.c)
target_include_directories(${PROJECT_NAME} PUBLIC include)
