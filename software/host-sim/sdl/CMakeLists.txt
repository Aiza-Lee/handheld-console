if(NOT TARGET SDL3::SDL3)
	message(FATAL_ERROR "SDL submodule was found, but SDL3::SDL3 was not exported.")
endif()

set(HOST_SIM_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(REPO_ROOT "${HOST_SIM_ROOT_DIR}/../..")

set(HOST_SIM_SDL_SOURCES
	main.cpp
	SdlPlatform.cpp
	SdlPlatformAudio.cpp
	SdlPlatformDisplay.cpp
	SdlPlatformInput.cpp
	SdlPlatformPower.cpp
	SdlPlatformStorage.cpp
	SdlPlatformTime.cpp
)

add_executable(host-sim-sdl
	${HOST_SIM_SDL_SOURCES}
)

# ── 资产管线 ──────────────────────────────────────
include(${REPO_ROOT}/cmake/AssetHelpers.cmake)

add_assets(TARGET host-sim-sdl
	ASSETS
		TEST_RED_SPRITE   1   ${REPO_ROOT}/assets/sprites/test_red.raw
)

add_sounds(TARGET host-sim-sdl
	SOUNDS
		BOOT             ${REPO_ROOT}/assets/sounds/boot.tone
		BGM_MENU         ${REPO_ROOT}/assets/sounds/bgm_menu.tone
		BGM_GUIDE        ${REPO_ROOT}/assets/sounds/bgm_guide.tone
		BGM_PLAYGROUND   ${REPO_ROOT}/assets/sounds/bgm_playground.tone
		BGM_SNAKE        ${REPO_ROOT}/assets/sounds/bgm_snake.tone
		BGM_PACMAN       ${REPO_ROOT}/assets/sounds/bgm_pacman.tone
		BGM_BREAKOUT     ${REPO_ROOT}/assets/sounds/bgm_breakout.tone
		BGM_INVADERS     ${REPO_ROOT}/assets/sounds/bgm_invaders.tone
		SFX_SELECT       ${REPO_ROOT}/assets/sounds/sfx_select.tone
		SFX_CONFIRM      ${REPO_ROOT}/assets/sounds/sfx_confirm.tone
		SFX_EAT          ${REPO_ROOT}/assets/sounds/sfx_eat.tone
		SFX_DEATH_SNAKE  ${REPO_ROOT}/assets/sounds/sfx_death_snake.tone
		SFX_CHOMP        ${REPO_ROOT}/assets/sounds/sfx_chomp.tone
		SFX_DEATH_PACMAN ${REPO_ROOT}/assets/sounds/sfx_death_pacman.tone
		SFX_POWER_PELLET ${REPO_ROOT}/assets/sounds/sfx_power_pellet.tone
		SFX_BRICK        ${REPO_ROOT}/assets/sounds/sfx_brick.tone
		SFX_PADDLE       ${REPO_ROOT}/assets/sounds/sfx_paddle.tone
		SFX_DEATH_BREAKOUT ${REPO_ROOT}/assets/sounds/sfx_death_breakout.tone
		SFX_SHOOT        ${REPO_ROOT}/assets/sounds/sfx_shoot.tone
		SFX_EXPLOSION    ${REPO_ROOT}/assets/sounds/sfx_explosion.tone
		SFX_UFO          ${REPO_ROOT}/assets/sounds/sfx_ufo.tone
)

target_include_directories(host-sim-sdl PRIVATE
	${HOST_SIM_ROOT_DIR}
)

target_link_libraries(host-sim-sdl PRIVATE
	game_core
	SDL3::SDL3
)
