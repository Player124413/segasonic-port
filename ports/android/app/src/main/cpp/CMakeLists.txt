# SegaSonic The Hedgehog — Android Native CMake Build
# Builds:
#   - libSDL2.so               (official SDL2 release-2.32.10, shared)
#   - libsegasonic_launcher.so (ROM CRC32 verifier & segasonic.assets extractor)
#   - libsegasonic_console.so  (Console Edition: 16:9 widescreen, menus, Time Attack, captions)
#   - libsegasonic_arcade.so   (Original Sega System 32 Arcade Edition)

cmake_minimum_required(VERSION 3.22)
project(segasonic_android C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

get_filename_component(SEGASONIC_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../.." ABSOLUTE)
if(NOT EXISTS "${SEGASONIC_ROOT}/CMakeLists.txt")
	message(FATAL_ERROR "Cannot locate SegaSonic repository root at ${SEGASONIC_ROOT}")
endif()

# Android 15+ / Google Play 16 KB page-size alignment across all built .so libraries (including libSDL2.so)
if(ANDROID)
	add_link_options(
		-Wl,-z,max-page-size=16384
		-Wl,-z,common-page-size=16384
		-Wl,--gc-sections
	)
endif()

# Aggressive optimization + section GC on Release/RelWithDebInfo builds
add_compile_options(
	"$<$<AND:$<COMPILE_LANGUAGE:C,CXX>,$<CONFIG:Release>>:-O3>"
	"$<$<AND:$<COMPILE_LANGUAGE:C,CXX>,$<CONFIG:RelWithDebInfo>>:-O3>"
	"$<$<COMPILE_LANGUAGE:C,CXX>:-ffunction-sections>"
	"$<$<COMPILE_LANGUAGE:C,CXX>:-fdata-sections>"
)

if(ANDROID AND (ANDROID_ABI STREQUAL "armeabi-v7a" OR CMAKE_ANDROID_ARCH_ABI STREQUAL "armeabi-v7a"))
	# Guarantee zero SIGBUS on 32-bit ARMv7 when V60 code accesses unaligned RAM/ROM addresses
	add_compile_options(
		"$<$<COMPILE_LANGUAGE:C,CXX>:SHELL:-mllvm -arm-assume-misaligned-load-store>"
		"$<$<COMPILE_LANGUAGE:C,CXX>:-fno-slp-vectorize>"
	)
endif()

# 1. Core SegaSonic targets (s32runtime, sonic_data, sonic_game, sonic_game_console)
set(SEGASONIC_REFERENCE_TOOLS OFF CACHE BOOL "" FORCE)
set(SEGASONIC_SCAFFOLD OFF CACHE BOOL "" FORCE)
set(SEGASONIC_STATIC_SDL2 OFF CACHE BOOL "" FORCE)
add_subdirectory("${SEGASONIC_ROOT}" "${CMAKE_BINARY_DIR}/segasonic_core")

# 2. SDL2 (release-2.32.10, matching vendored org.libsdl.app Java bindings)
set(SDL_SHARED ON CACHE BOOL "" FORCE)
set(SDL_STATIC OFF CACHE BOOL "" FORCE)
set(SDL_TEST OFF CACHE BOOL "" FORCE)
set(SDL_TESTS OFF CACHE BOOL "" FORCE)

set(SEGASONIC_SDL2_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../../sdl2" CACHE PATH "Optional local SDL2 checkout")
if(EXISTS "${SEGASONIC_SDL2_DIR}/CMakeLists.txt")
	add_subdirectory("${SEGASONIC_SDL2_DIR}" "${CMAKE_BINARY_DIR}/sdl2_build")
else()
	include(FetchContent)
	FetchContent_Declare(
		SDL2
		GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
		GIT_TAG        release-2.32.10
		GIT_SHALLOW    TRUE
	)
	FetchContent_MakeAvailable(SDL2)
endif()

find_library(ANDROID_LIB android REQUIRED)
find_library(LOG_LIB log REQUIRED)
find_library(GLES2_LIB GLESv2 REQUIRED)

# 3. Lightweight Launcher JNI library (main UI process: ROM verification & asset pack extraction)
add_library(segasonic_launcher SHARED
	launcher_jni.c
)
target_include_directories(segasonic_launcher PRIVATE
	"${SEGASONIC_ROOT}/runtime/include"
)
target_link_libraries(segasonic_launcher PRIVATE
	sonic_data
	${LOG_LIB}
)

# Common Android port sources shared between Console and Arcade editions
set(ANDROID_PORT_COMMON_SOURCES
	etc2.c
	ui_atlas.c
	touch_controls.c
	gles_renderer.c
	"${SEGASONIC_ROOT}/ports/sdl2/settings.c"
)

# 4. SegaSonic Console Edition shared library (:game process)
add_library(segasonic_console SHARED
	android_main.c
	${ANDROID_PORT_COMMON_SOURCES}
	"${SEGASONIC_ROOT}/ports/sdl2/actions.c"
)
target_compile_definitions(segasonic_console PRIVATE SEGASONIC_CONSOLE)
target_include_directories(segasonic_console PRIVATE
	"${CMAKE_CURRENT_SOURCE_DIR}"
	"${SEGASONIC_ROOT}/runtime/include"
	"${SEGASONIC_ROOT}/console"
	"${SEGASONIC_ROOT}/ports/sdl2"
)
target_link_libraries(segasonic_console PRIVATE
	sonic_game_console
	sonic_data
	SDL2::SDL2
	${GLES2_LIB}
	${ANDROID_LIB}
	${LOG_LIB}
	m
)

# 5. SegaSonic Arcade Edition shared library (:game process)
add_library(segasonic_arcade SHARED
	android_main.c
	${ANDROID_PORT_COMMON_SOURCES}
)
target_include_directories(segasonic_arcade PRIVATE
	"${CMAKE_CURRENT_SOURCE_DIR}"
	"${SEGASONIC_ROOT}/runtime/include"
	"${SEGASONIC_ROOT}/ports/sdl2"
)
target_link_libraries(segasonic_arcade PRIVATE
	sonic_game
	sonic_data
	SDL2::SDL2
	${GLES2_LIB}
	${ANDROID_LIB}
	${LOG_LIB}
	m
)
