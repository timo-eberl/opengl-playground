cmake_minimum_required(VERSION 3.5)
project(ron)
set(CMAKE_CXX_STANDARD 20) # Require C++20

# this cmake project builds the library

# the example program(s) will be built by default, they can be disabled like this:
# set(RON_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# the default asset directory is in the same folder ron is
# when using a custom directory make sure that the default assets (assets/default) are included
# you can set a custom directory like this:
# set(RON_ASSET_DIRECTORY /path/to/assets/)

option(RON_BUILD_EXAMPLES "Build the Ron example programs" ON)

if (NOT DEFINED RON_ASSET_DIRECTORY)
	set(RON_ASSET_DIRECTORY ${PROJECT_SOURCE_DIR}/assets/)
	message("RON_ASSET_DIRECTORY was not already defined. Default directory will be used: " ${RON_ASSET_DIRECTORY})
endif()

# dependencies
	# glfw - create windows, create OpenGL context, handle input
	# disable tests, examples and documentation
	set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
	set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
	set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
	add_subdirectory(dependencies/glfw)

	# glad - OpenGL Loader-Generator
	add_subdirectory(dependencies/glad)

	# stb_image - image loader
	add_subdirectory(dependencies/stb_image)

	# glm - OpenGL Mathematics with shader-like syntax
	add_subdirectory(dependencies/glm)

	# cgltf - gltf loader
	add_subdirectory(dependencies/cgltf)

	# mikktspace - tangent generation
	add_subdirectory(dependencies/mikktspace)

# this project
	add_compile_definitions(_ASSETS_DIR=\"${RON_ASSET_DIRECTORY}\")
	set(SOURCES
		src/log.cpp
		src/shader_program.cpp
		src/uniforms.cpp
		src/texture.cpp
		src/perspective_camera.cpp
		src/camera_viewport_controls.cpp
		src/gltf.cpp
		src/mesh_node.cpp
		src/scene.cpp
		src/opengl_renderer.cpp
		src/opengl_shader_program.cpp
		src/opengl_geometry.cpp
		src/opengl_texture.cpp
		src/opengl_axes_renderer.cpp
		src/assets.cpp
		src/tangent_generation.cpp
	)
	add_library(${PROJECT_NAME} ${SOURCES})
	target_include_directories(${PROJECT_NAME} PRIVATE src)
	target_include_directories(${PROJECT_NAME} PUBLIC include)
	# statically link dependencies
	target_link_libraries(${PROJECT_NAME} PUBLIC glfw)
	target_link_libraries(${PROJECT_NAME} PUBLIC glad)
	target_link_libraries(${PROJECT_NAME} PUBLIC stb_image)
	target_link_libraries(${PROJECT_NAME} PUBLIC glm)
	target_link_libraries(${PROJECT_NAME} PUBLIC cgltf)
	target_link_libraries(${PROJECT_NAME} PUBLIC mikktspace)

# example application
	if (RON_BUILD_EXAMPLES)
		add_executable(${PROJECT_NAME}_example example/src/main.cpp)
		# statically link ron library
		target_link_libraries(${PROJECT_NAME}_example PRIVATE ${PROJECT_NAME})
	endif()
