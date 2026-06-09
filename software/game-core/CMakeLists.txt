set(GAME_CORE_SOURCES
	src/core/runtime/ScreenRunner.cpp
	src/scenes/boot/BootScreen.cpp
	src/scenes/menu/MenuScreen.cpp
	# Temporarily disabled to reduce firmware size (exceeds flash by 17KB):
	#	src/scenes/playground/PlaygroundGuideScreen.cpp
	#	src/scenes/playground/PlaygroundScreen.cpp
		src/scenes/snake/SnakeScreen.cpp
		src/scenes/pacman/PacmanScreen.cpp
		src/scenes/breakout/BreakoutScreen.cpp
		src/scenes/invaders/InvadersScreen.cpp
		src/scenes/settings/SettingsScreen.cpp
		src/scenes/growball/GrowBallScreen.cpp
	src/scenes/growball/GrowBallWorld.cpp
	src/scenes/tetris/TetrisScreen.cpp
	src/scenes/game2048/Game2048Screen.cpp
	src/scenes/pong/PongScreen.cpp
		src/scenes/mp3/Mp3PlayerScreen.cpp
	src/scenes/developer/DeveloperScreen.cpp
)

add_library(game_core STATIC
	${GAME_CORE_SOURCES}
)

target_include_directories(game_core PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Size-optimization flags. Applied to game_core only so the host-sim SDL
# platform code (which uses exceptions) is unaffected. Firmware (MSPM0)
# is built with its own toolchain; these flags approximate that here
# for size-budget measurement.
#
# Note: LTO is intentionally omitted because the host-sim link step
# uses the system ld which can't read LLVM bitcode archives. Re-enable
# LTO by also wiring up lld (`-fuse-ld=lld`) for the test executables
# and adding -flto to their link options.
target_compile_options(game_core PRIVATE
	-Os
	-ffunction-sections
	-fdata-sections
	-fno-rtti
	-fno-exceptions
)
target_link_options(game_core PRIVATE
	-Wl,--gc-sections
)

add_subdirectory(tests)
