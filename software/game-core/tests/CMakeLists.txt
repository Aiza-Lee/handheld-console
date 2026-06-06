function(add_game_core_smoke_test target_name source_file)
	add_executable(${target_name}
		${source_file}
	)

	target_link_libraries(${target_name} PRIVATE game_core)

	target_include_directories(${target_name} PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/..
	)

	add_test(
		NAME ${target_name}
		COMMAND ${target_name}
	)
endfunction()

add_game_core_smoke_test(game-core-smoke ScreenRunnerSmoke.cpp)
add_game_core_smoke_test(game-core-text-smoke TextRendererSmoke.cpp)
add_game_core_smoke_test(game-core-snake-smoke SnakeSmoke.cpp)
add_game_core_smoke_test(game-core-pacman-smoke PacmanSmoke.cpp)
add_game_core_smoke_test(game-core-breakout-smoke BreakoutSmoke.cpp)
add_game_core_smoke_test(game-core-growball-smoke GrowBallSmoke.cpp)
add_game_core_smoke_test(game-core-tetris-smoke TetrisSmoke.cpp)
add_game_core_smoke_test(game-core-2048-smoke Game2048Smoke.cpp)
add_game_core_smoke_test(game-core-pong-smoke PongSmoke.cpp)
add_game_core_smoke_test(game-core-mp3-smoke Mp3Smoke.cpp)
add_game_core_smoke_test(game-core-developer-smoke DeveloperSmoke.cpp)
add_game_core_smoke_test(game-core-boot-dev-trigger-smoke BootDevTriggerSmoke.cpp)
add_game_core_smoke_test(game-core-storage-smoke StorageSmoke.cpp)
