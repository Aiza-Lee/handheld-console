cmake_minimum_required(VERSION 3.28)

project(host-sim LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()

add_subdirectory(
	${CMAKE_CURRENT_SOURCE_DIR}/../game-core
	${CMAKE_CURRENT_BINARY_DIR}/game-core
)

add_executable(host-sim
	app/main.cpp
)

target_link_libraries(host-sim PRIVATE game_core)

target_include_directories(host-sim PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/../game-core
)

set(SDL_SUBMODULE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../third_party/SDL)

if(EXISTS ${SDL_SUBMODULE_DIR}/CMakeLists.txt)
	add_subdirectory(
		${SDL_SUBMODULE_DIR}
		${CMAKE_CURRENT_BINARY_DIR}/third_party/sdl
		EXCLUDE_FROM_ALL
	)
	add_subdirectory(sdl)
else()
	message(STATUS "SDL submodule not found at ${SDL_SUBMODULE_DIR}; skipping host-sim-sdl target.")
endif()
