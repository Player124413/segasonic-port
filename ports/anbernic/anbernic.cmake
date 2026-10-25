# Name:        anbernic.cmake
# Description: the build for Anbernic's H700 handhelds (the RG40XX H...) on their stock firmware: the console
#              game alone, as Roms/PORTS' SegaSonic.sh and its folder segasonic/
# Version:     0.1.0
# Created:     23-10-2026
# Modified:    25-10-2026
# Author:      Sebastian Toyos
#
# Included by CMakeLists.txt with SEGASONIC_ANBERNIC (and the toolchain zig_aarch64.cmake: aarch64, glibc
# 2.35). The game is the console build with its picture and controls fixed (SEGASONIC_HANDHELD), the
# handheld's MENU and volume keys (SEGASONIC_HANDHELD_KEYS), CONTROLS in OPTIONS, and with the firmware's SDL2: the executable is linked against a stand-in of its name (sdl2_stub.c) and SDL's headers
# (SEGASONIC_SDL2_INCLUDE, by default the development package in build/downloads/sdl2). The handheld makes
# nothing from the ROM set: its folder gets the assets pack this computer has (build/bin/segasonic.assets),
# taken out of the player's ROM set here.

target_compile_definitions(sonic_game_console PUBLIC SEGASONIC_HANDHELD SEGASONIC_HANDHELD_KEYS SEGASONIC_CONTROLS_PAGE)

set(SEGASONIC_SDL2_INCLUDE "" CACHE PATH "SDL2's headers (the folder with SDL.h)")
if(NOT SEGASONIC_SDL2_INCLUDE)
	file(GLOB SONIC_SDL2_INCLUDES ${CMAKE_SOURCE_DIR}/build/downloads/sdl2/SDL2-*/include)
	list(GET SONIC_SDL2_INCLUDES 0 SEGASONIC_SDL2_INCLUDE)
endif()
if(NOT EXISTS ${SEGASONIC_SDL2_INCLUDE}/SDL.h)
	message(FATAL_ERROR "No SDL.h in SEGASONIC_SDL2_INCLUDE (${SEGASONIC_SDL2_INCLUDE})")
endif()

# the stand-in for libSDL2-2.0.so.0, beside the build's own files (never in the package)
add_library(SDL2_device SHARED ports/anbernic/sdl2_stub.c)
set_target_properties(SDL2_device PROPERTIES OUTPUT_NAME SDL2-2.0 SUFFIX ".so.0" NO_SONAME ON
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/sdl2_device)
target_link_options(SDL2_device PRIVATE -Wl,-soname,libSDL2-2.0.so.0)

set(SONIC_ANBERNIC_OUT ${CMAKE_BINARY_DIR}/PORTS)
add_executable(SegaSonic ports/sdl2/main.c ports/sdl2/settings.c ports/sdl2/actions.c)
target_include_directories(SegaSonic PRIVATE ${SEGASONIC_SDL2_INCLUDE})
target_link_libraries(SegaSonic PRIVATE sonic_game_console sonic_data SDL2_device m)
set_target_properties(SegaSonic PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${SONIC_ANBERNIC_OUT}/segasonic
	SKIP_BUILD_RPATH ON)    # (no path of this computer's in it: the handheld's loader finds its SDL2)
foreach(config IN ITEMS _DEBUG _RELEASE _RELWITHDEBINFO _MINSIZEREL)
	set_target_properties(SegaSonic PROPERTIES RUNTIME_OUTPUT_DIRECTORY${config} ${SONIC_ANBERNIC_OUT}/segasonic)
endforeach()

# the launcher beside the folder; in it CONTROLS' button pictures (input_prompts.bmp) and this computer's assets
# pack
set(SONIC_ANBERNIC_PACK ${SONIC_BIN_DIR}/segasonic.assets)
add_custom_command(TARGET SegaSonic POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/ports/anbernic/SegaSonic.sh ${SONIC_ANBERNIC_OUT}
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/ports/sdl2/prompts/input_prompts.bmp
		${SONIC_ANBERNIC_OUT}/segasonic
	VERBATIM)
if(EXISTS ${SONIC_ANBERNIC_PACK})
	add_custom_command(TARGET SegaSonic POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SONIC_ANBERNIC_PACK} ${SONIC_ANBERNIC_OUT}/segasonic
		VERBATIM)
else()
	message(STATUS "No assets pack at ${SONIC_ANBERNIC_PACK}: the handheld's folder gets the game alone")
endif()
