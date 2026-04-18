cmake_minimum_required(VERSION 3.28)

project(host-sim LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(GAME_CORE_SOURCES
	../game-core/src/core/runtime/SceneRunner.cpp
	../game-core/src/core/runtime/SceneFactory.cpp
	../game-core/src/scenes/menu/MenuScene.cpp
	../game-core/src/scenes/playground/PlaygroundScene.cpp
)

add_library(game_core STATIC
	${GAME_CORE_SOURCES}
)

target_include_directories(game_core PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/../game-core/include
)

add_executable(host-sim
	main.cpp
)

target_link_libraries(host-sim PRIVATE game_core)

target_include_directories(host-sim PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/../game-core
)

add_executable(game-core-smoke
	../game-core/tests/SceneRunnerSmoke.cpp
)

target_link_libraries(game-core-smoke PRIVATE game_core)

target_include_directories(game-core-smoke PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/../game-core
)

enable_testing()

add_test(
	NAME game-core-smoke
	COMMAND game-core-smoke
)

add_executable(game-core-text-smoke
	../game-core/tests/TextRendererSmoke.cpp
)

target_link_libraries(game-core-text-smoke PRIVATE game_core)

target_include_directories(game-core-text-smoke PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/../game-core
)

add_test(
	NAME game-core-text-smoke
	COMMAND game-core-text-smoke
)

set(SDL_SUBMODULE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../third_party/SDL)

if(EXISTS ${SDL_SUBMODULE_DIR}/CMakeLists.txt)
	add_subdirectory(
		${SDL_SUBMODULE_DIR}
		${CMAKE_CURRENT_BINARY_DIR}/third_party/sdl
		EXCLUDE_FROM_ALL
	)

	set(HOST_SIM_SDL_SOURCES
		sdl/main.cpp
		sdl/SdlPlatform.cpp
		sdl/SdlPlatformAudio.cpp
		sdl/SdlPlatformDisplay.cpp
		sdl/SdlPlatformInput.cpp
		sdl/SdlPlatformPower.cpp
		sdl/SdlPlatformStorage.cpp
		sdl/SdlPlatformTime.cpp
	)

	add_executable(host-sim-sdl
		${HOST_SIM_SDL_SOURCES}
	)

	target_include_directories(host-sim-sdl PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}
	)

	if(TARGET SDL3::SDL3)
		target_link_libraries(host-sim-sdl PRIVATE game_core SDL3::SDL3)
	else()
		message(FATAL_ERROR "SDL submodule was found, but SDL3::SDL3 was not exported.")
	endif()
else()
	message(STATUS "SDL submodule not found at ${SDL_SUBMODULE_DIR}; skipping host-sim-sdl target.")
endif()
