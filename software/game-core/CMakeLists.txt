set(GAME_CORE_SOURCES
	src/core/runtime/ScreenRunner.cpp
	src/core/runtime/ScreenFactory.cpp
	src/core/assets/BuiltinAssetProvider.cpp
	src/scenes/boot/BootScreen.cpp
	src/scenes/menu/MenuScreen.cpp
	src/scenes/playground/PlaygroundGuideScreen.cpp
		src/scenes/playground/PlaygroundScreen.cpp
		src/scenes/snake/SnakeScreen.cpp
		src/scenes/pacman/PacmanScreen.cpp
		src/scenes/breakout/BreakoutScreen.cpp
		src/scenes/invaders/InvadersScreen.cpp
		src/scenes/settings/SettingsScreen.cpp
		src/scenes/growball/GrowBallScreen.cpp
	src/scenes/growball/GrowBallWorld.cpp
)

add_library(game_core STATIC
	${GAME_CORE_SOURCES}
)

target_include_directories(game_core PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_subdirectory(tests)
