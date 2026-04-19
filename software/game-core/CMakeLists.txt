set(GAME_CORE_SOURCES
	src/core/runtime/SceneRunner.cpp
	src/core/runtime/SceneFactory.cpp
	src/scenes/menu/MenuScene.cpp
	src/scenes/playground/PlaygroundScene.cpp
)

add_library(game_core STATIC
	${GAME_CORE_SOURCES}
)

target_include_directories(game_core PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_subdirectory(tests)
