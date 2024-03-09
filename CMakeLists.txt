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
# cgltf - gltf loader
add_subdirectory(cgltf)

# our project
add_compile_definitions(_ASSETS_DIR=\"${PROJECT_SOURCE_DIR}/assets/\")
set(SOURCES
	src/glpg/log.cpp
	src/glpg/shader_program.cpp
	src/glpg/uniforms.cpp
	src/glpg/texture.cpp
	src/glpg/perspective_camera.cpp
	src/glpg/camera_viewport_controls.cpp
	src/glpg/gltf.cpp
	src/glpg/mesh_node.cpp
	src/glpg/scene.cpp
	src/glpg/opengl_renderer.cpp
	src/glpg/opengl_geometry_gpu_data.cpp
	src/glpg/opengl_shader_program_gpu_data.cpp
	src/glpg/opengl_axes_renderer.cpp
	src/glpg/opengl_uniforms.cpp
	src/glpg/assets.cpp
	src/glpg/examples/hello_triangle/hello_triangle.cpp
	src/glpg/examples/demo/demo.cpp
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
target_link_libraries(${PROJECT_NAME} PRIVATE cgltf)
