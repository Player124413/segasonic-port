# Name:        CMakeLists.txt
# Description: build of the SegaSonic runtime and its development tools
# Version:     0.1.0
# Created:     11-10-2026
# Modified:    15-10-2026
# Author:      Sebastian Toyos
cmake_minimum_required(VERSION 3.20)
project(segasonic C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Every executable lands in build/bin (SONIC_BIN_DIR), whatever the generator or configuration
set(SONIC_BIN_DIR ${CMAKE_SOURCE_DIR}/build/bin CACHE PATH "Where the executables go")
foreach(config IN ITEMS "" _DEBUG _RELEASE _RELWITHDEBINFO _MINSIZEREL)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY${config} ${SONIC_BIN_DIR})
endforeach()

if(MSVC)
	add_compile_options(/W3 /utf-8)
	add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

# System 32 board model: part of the game (sound.c is the Z80's side of the board, video.c draws the frames,
# script.c drives the controls in test runs)
add_library(s32runtime STATIC
	runtime/src/machine.c
	runtime/src/io.c
	runtime/src/script.c
	runtime/src/sound.c
	runtime/src/sound_chips.c
	runtime/src/rf5c68.c
	runtime/src/ym3438.c
	runtime/src/video.c
	runtime/src/memory_census.c
)
target_include_directories(s32runtime PUBLIC runtime/include)

# The game's data, carried inside the executables that run it: the board's ROM regions, written from the
# player's ROM set (rom/, its files or its .zip; SONIC_ROM picks another) by tools/embed_data.py when the
# set changes; when assets/ holds the extracted assets (extract_assets.py), they stand for their part of
# the ROM, and a change to them goes into the game. The C it writes stays in the build folder: the ROM
# never goes into the repository
find_package(Python3 REQUIRED COMPONENTS Interpreter)
set(SONIC_ROM ${CMAKE_SOURCE_DIR}/rom CACHE PATH "The ROM set the game's data is written from")
if(IS_DIRECTORY ${SONIC_ROM})
	file(GLOB SONIC_ROM_FILES ${SONIC_ROM}/*)
else()
	set(SONIC_ROM_FILES ${SONIC_ROM})
endif()
file(GLOB_RECURSE SONIC_ASSET_FILES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/assets/*)
file(GLOB SONIC_FORMAT_TOOLS CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/tools/formats/*.py)
set(SONIC_DATA_DIR ${CMAKE_BINARY_DIR}/data)
set(SONIC_DATA_SOURCES
	${SONIC_DATA_DIR}/program.c ${SONIC_DATA_DIR}/tiles.c ${SONIC_DATA_DIR}/sprites.c ${SONIC_DATA_DIR}/sound.c)
add_custom_command(OUTPUT ${SONIC_DATA_SOURCES}
	COMMAND Python3::Interpreter ${CMAKE_SOURCE_DIR}/tools/embed_data.py ${SONIC_ROM} ${SONIC_DATA_DIR}
	DEPENDS ${SONIC_ROM_FILES} ${SONIC_ASSET_FILES} ${SONIC_FORMAT_TOOLS} ${CMAKE_SOURCE_DIR}/assets.json
		${CMAKE_SOURCE_DIR}/extract_assets.py ${CMAKE_SOURCE_DIR}/tools/embed_data.py
		${CMAKE_SOURCE_DIR}/tools/sonic_rom.py ${CMAKE_SOURCE_DIR}/tools/asset_formats.py
		${CMAKE_SOURCE_DIR}/tools/data_formats.py
	COMMENT "Writing the game's data from the ROM set and the assets")
add_library(sonic_data STATIC ${SONIC_DATA_SOURCES})
target_include_directories(sonic_data PUBLIC runtime/include)

# The translation scaffold the decompiler grew from: the V60 runtime plus the C tools/recompile.py writes
# from the user's ROM into build/generated, instruction by instruction (for comparisons only)
option(SEGASONIC_SCAFFOLD "Build the V60 translation scaffold (sonic_headless)" OFF)
file(GLOB SONIC_GENERATED CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/build/generated/*.c)
if(SEGASONIC_SCAFFOLD AND SONIC_GENERATED)
	add_library(sonic_program STATIC runtime/src/v60.c ${SONIC_GENERATED})
	target_include_directories(sonic_program PUBLIC runtime/include build/generated)
	target_link_libraries(sonic_program PUBLIC s32runtime)
	if(MSVC)
		target_compile_options(sonic_program PRIVATE /bigobj)
	endif()

	add_executable(sonic_headless ports/headless/main.c)
	target_link_libraries(sonic_headless PRIVATE sonic_program sonic_data)
elseif(SEGASONIC_SCAFFOLD)
	message(STATUS "No generated program yet: run tools/recompile.py to build the scaffold")
endif()

# The game: its runtime plus its C, src/ (tools/decompile.py writes it; SONIC_DECOMPILED_DIR picks another
# copy, such as one written with --trace for debugging)
set(SONIC_DECOMPILED_DIR ${CMAKE_SOURCE_DIR}/src CACHE PATH "The game's C (tools/decompile.py)")
file(GLOB_RECURSE SONIC_DECOMPILED CONFIGURE_DEPENDS ${SONIC_DECOMPILED_DIR}/*.c)
if(SONIC_DECOMPILED)
	add_library(sonic_game STATIC runtime/src/game.c runtime/src/sound_cpu.c ${SONIC_DECOMPILED})
	target_include_directories(sonic_game PUBLIC runtime/include ${SONIC_DECOMPILED_DIR} ${SONIC_DECOMPILED_DIR}/sound)
	target_link_libraries(sonic_game PUBLIC s32runtime)
	if(MSVC)
		target_compile_options(sonic_game PRIVATE /bigobj)
	else()
		# variables are typed lvalues over the work RAM bytes (game/support.h)
		target_compile_options(sonic_game PRIVATE -fno-strict-aliasing)
	endif()

	# a development aid: every mem_read and mem_write of the game's C counted by its line
	# (sonic_game_headless --memory-census FILE; runtime/src/memory_census.c)
	option(SONIC_MEMORY_CENSUS "Count the game's memory accesses by line of C" OFF)
	if(SONIC_MEMORY_CENSUS)
		target_compile_definitions(sonic_game PUBLIC S32_MEMORY_CENSUS)
	endif()

	add_executable(sonic_game_headless ports/headless/main.c)
	target_link_libraries(sonic_game_headless PRIVATE sonic_game sonic_data)

	# The game in a window (ports/sdl2): SDL2 from the system, or the development package unpacked in
	# build/downloads/sdl2 (SDL2_DIR picks another)
	file(GLOB SONIC_SDL2_UNPACKED ${CMAKE_SOURCE_DIR}/build/downloads/sdl2/SDL2-*/cmake)
	find_package(SDL2 CONFIG QUIET HINTS ${SONIC_SDL2_UNPACKED})
	if(SDL2_FOUND)
		add_executable(sonic WIN32 ports/sdl2/main.c ports/sdl2/settings.c)
		target_link_libraries(sonic PRIVATE sonic_game sonic_data SDL2::SDL2 SDL2::SDL2main)
		if(WIN32)
			add_custom_command(TARGET sonic POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:SDL2::SDL2> $<TARGET_FILE_DIR:sonic>)
		endif()
	else()
		message(STATUS "No SDL2: the window port (sonic) is not built")
	endif()
else()
	message(STATUS "No game C in ${SONIC_DECOMPILED_DIR}: run tools/decompile.py")
endif()

# Development tools built on MAME's V60 code; never linked into the game
option(SEGASONIC_REFERENCE_TOOLS "Build the MAME-based reference tools" ON)
if(SEGASONIC_REFERENCE_TOOLS)
	add_executable(oracle
		tools/reference/oracle.cpp
		tools/reference/mame/v60d.cpp
	)
	target_include_directories(oracle PRIVATE tools/reference/stub tools/reference/mame)

	# MAME's Z80 disassembler, the reference tools/z80/disassembler.py is compared with
	add_executable(oracle_z80
		tools/reference/oracle_z80.cpp
		tools/reference/mame/z80/z80dasm.cpp
	)
	target_include_directories(oracle_z80 PRIVATE tools/reference/stub tools/reference/mame/z80)

	# MAME's System 32 video drawing the board model's frames: the frames the game's video has to match
	add_library(s32video_reference STATIC tools/reference/video/s32video.cpp)
	target_include_directories(s32video_reference PUBLIC tools/reference/video PRIVATE runtime/include)
	if(MSVC)
		target_compile_options(s32video_reference PRIVATE /wd4244 /wd4267 /wd4100 /wd4146)
	endif()

	# MAME's Z80 and RF5C68 and ymfm's YM3438 on the board model: the sound the game's has to match. The Z80
	# core's steps are written from z80.lst at build time, as MAME does
	set(SONIC_Z80_STEPS ${CMAKE_BINARY_DIR}/generated/cpu/z80/z80.hxx)
	add_custom_command(OUTPUT ${SONIC_Z80_STEPS}
		COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/generated/cpu/z80
		COMMAND Python3::Interpreter ${CMAKE_SOURCE_DIR}/tools/reference/mame/z80/z80make.py z80
			${CMAKE_SOURCE_DIR}/tools/reference/mame/z80/z80.lst ${SONIC_Z80_STEPS}
		DEPENDS tools/reference/mame/z80/z80make.py tools/reference/mame/z80/z80.lst
		VERBATIM)
	add_library(s32sound_reference STATIC
		tools/reference/sound/s32sound.cpp
		tools/reference/ymfm/ymfm_opn.cpp
		tools/reference/ymfm/ymfm_adpcm.cpp
		tools/reference/ymfm/ymfm_ssg.cpp
		${SONIC_Z80_STEPS}
	)
	target_include_directories(s32sound_reference PUBLIC tools/reference/sound
		PRIVATE tools/reference/ymfm runtime/include ${CMAKE_BINARY_DIR}/generated)
	if(MSVC)
		target_compile_options(s32sound_reference PRIVATE /bigobj /wd4244 /wd4267 /wd4100 /wd4146 /wd4127)
	endif()

	add_executable(s32ref
		tools/reference/s32ref.cpp
		tools/reference/mame/v60.cpp
		tools/reference/mame/v60d.cpp
	)
	target_include_directories(s32ref PRIVATE tools/reference/stub tools/reference/mame)
	target_link_libraries(s32ref PRIVATE s32runtime sonic_data s32video_reference s32sound_reference)
	if(MSVC)
		# MAME's core marks error handlers [[noreturn]] even though they return uint32_t
		target_compile_options(s32ref PRIVATE /wd4646 /wd4244)
	endif()

	# The game's YM3438 and RF5C68 against ymfm's and MAME's, sample for sample, over s32ref's chip write
	# logs and synthetic sequences; the game's C is compiled in directly
	add_executable(chipcheck
		tools/reference/sound/chipcheck.cpp
		runtime/src/ym3438.c
		runtime/src/rf5c68.c
		tools/reference/ymfm/ymfm_opn.cpp
		tools/reference/ymfm/ymfm_adpcm.cpp
		tools/reference/ymfm/ymfm_ssg.cpp
	)
	target_include_directories(chipcheck PRIVATE tools/reference/sound tools/reference/ymfm runtime/include)
	if(MSVC)
		target_compile_options(chipcheck PRIVATE /wd4244 /wd4267 /wd4100 /wd4146 /wd4127)
	endif()
endif()
