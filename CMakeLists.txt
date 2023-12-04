cmake_minimum_required(VERSION 3.5)
project(opengl-playground)
set(CMAKE_CXX_STANDARD 20) # Require C++20

# dependencies
# glfw
# disable tests, examples and documentation
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw)
# glad
add_subdirectory(glad)

# our project
set(SOURCES
	src/glpg/template_app/template_app.cpp
	src/glpg/hello_triangle/hello_triangle.cpp
)
add_executable(${PROJECT_NAME}
	src/main.cpp
	${SOURCES}
)
target_include_directories(${PROJECT_NAME} PRIVATE src)
# statically link dependencies
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
target_link_libraries(${PROJECT_NAME} PRIVATE glad)
