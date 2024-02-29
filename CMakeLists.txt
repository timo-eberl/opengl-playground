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
# stb_image
add_subdirectory(stb_image)
# OpenGL Mathematics (GLM)
add_subdirectory(glm)

# our project
add_compile_definitions(_ASSETS_DIR=\"${PROJECT_SOURCE_DIR}/assets/\")
set(SOURCES
	src/glpg/shader_program.cpp
	src/glpg/uniforms.cpp
	src/glpg/texture.cpp
	src/glpg/perspective_camera.cpp
	src/glpg/camera_orbit_controls.cpp
	src/glpg/template_app/template_app.cpp
	src/glpg/hello_triangle/hello_triangle.cpp
	src/glpg/shader_intro/shader_intro.cpp
	src/glpg/textures_app/textures_app.cpp
	src/glpg/coordinate_sytems_app/coordinate_sytems_app.cpp
)
add_executable(${PROJECT_NAME}
	src/main.cpp
	${SOURCES}
)
target_include_directories(${PROJECT_NAME} PRIVATE src)
# statically link dependencies
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
target_link_libraries(${PROJECT_NAME} PRIVATE glad)
target_link_libraries(${PROJECT_NAME} PRIVATE stb_image)
target_link_libraries(${PROJECT_NAME} PRIVATE glm)
