set(GAME_CORE_SOURCES
	src/core/runtime/ScreenRunner.cpp
	src/core/runtime/ScreenFactory.cpp
	src/scenes/menu/MenuScreen.cpp
	src/scenes/playground/PlaygroundScreen.cpp
)

add_library(game_core STATIC
	${GAME_CORE_SOURCES}
)

target_include_directories(game_core PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_subdirectory(tests)
