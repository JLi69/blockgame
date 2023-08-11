cmake_minimum_required(VERSION 3.11)

project(blockgame)

find_package(OpenGL REQUIRED)

aux_source_directory(src source)
aux_source_directory(glad/src glad)

include_directories(glad/include)

add_executable(
	${PROJECT_NAME}

	${source}
	${glad}
)

target_link_libraries(${PROJECT_NAME} glfw3 GL)
