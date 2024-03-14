cmake_minimum_required(VERSION 3.5)
project(glad)

set(SOURCES
	src/glad.c
)
add_library(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC include)
