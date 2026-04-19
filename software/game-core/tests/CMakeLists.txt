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

add_game_core_smoke_test(game-core-smoke SceneRunnerSmoke.cpp)
add_game_core_smoke_test(game-core-text-smoke TextRendererSmoke.cpp)
