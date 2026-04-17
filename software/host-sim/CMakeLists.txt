cmake_minimum_required(VERSION 3.28)

project(host-sim LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(game_core STATIC
	../game-core/src/app/App.cpp
	../game-core/src/games/menu/MenuGame.cpp
)

target_include_directories(game_core PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/../game-core/include
)

add_executable(host-sim
	main.cpp
)

target_link_libraries(host-sim PRIVATE game_core)

add_executable(game-core-smoke
	../game-core/tests/AppSmoke.cpp
)

target_link_libraries(game-core-smoke PRIVATE game_core)

enable_testing()

add_test(
	NAME game-core-smoke
	COMMAND game-core-smoke
)
