cmake_minimum_required(VERSION 3.11)

project(blockgame)

set(CMAKE_CXX_FLAGS -O2)
find_package(OpenGL REQUIRED)

aux_source_directory(src source)
aux_source_directory(lib/glad/src glad)

include_directories(lib/glad/include)
include_directories(lib/stb_image)

add_executable(
	${PROJECT_NAME}

	${source}
	${glad}
)

target_link_libraries(${PROJECT_NAME} glfw3 GL)
