if(NOT TARGET SDL3::SDL3)
	message(FATAL_ERROR "SDL submodule was found, but SDL3::SDL3 was not exported.")
endif()

set(HOST_SIM_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)

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

target_include_directories(host-sim-sdl PRIVATE
	${HOST_SIM_ROOT_DIR}
)

target_link_libraries(host-sim-sdl PRIVATE
	game_core
	SDL3::SDL3
)
