# Name:        zig_aarch64.cmake
# Description: CMake's toolchain for Anbernic's H700 handhelds' stock firmware (aarch64 Linux, glibc 2.35):
#              Zig's C compiler, which builds for any glibc with nothing else installed
# Version:     0.1.0
# Created:     23-10-2026
# Modified:    23-10-2026
# Author:      Sebastian Toyos
#
# Usage: cmake -S . -B build/anbernic -G Ninja -DCMAKE_TOOLCHAIN_FILE=ports/anbernic/zig_aarch64.cmake
#        -DSEGASONIC_ANBERNIC=ON [-DZIG=path/to/zig]
# Zig (ziglang.org) is found as ZIG, on the PATH or unpacked in build/downloads/zig. CMake runs one program for
# each tool, so the toolchain writes small scripts that run Zig's cc, c++, ar and ranlib for the target.

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)
set(SONIC_ZIG_TARGET aarch64-linux-gnu.2.35)    # the stock firmware's Ubuntu 22.04: glibc 2.35

if(NOT ZIG)
	if(CMAKE_HOST_WIN32)
		file(GLOB SONIC_ZIG_UNPACKED ${CMAKE_CURRENT_LIST_DIR}/../../build/downloads/zig/*/zig.exe)
	else()
		file(GLOB SONIC_ZIG_UNPACKED ${CMAKE_CURRENT_LIST_DIR}/../../build/downloads/zig/*/zig)
	endif()
	if(SONIC_ZIG_UNPACKED)
		list(GET SONIC_ZIG_UNPACKED 0 ZIG)
	else()
		find_program(ZIG zig REQUIRED)
	endif()
endif()
set(ZIG ${ZIG} CACHE FILEPATH "Zig, the compiler for the handheld")

set(SONIC_ZIG_TOOLS ${CMAKE_BINARY_DIR}/zig_tools)
file(MAKE_DIRECTORY ${SONIC_ZIG_TOOLS})
foreach(tool cc c++ ar ranlib)
	if(tool STREQUAL "cc" OR tool STREQUAL "c++")
		set(SONIC_ZIG_ARGUMENTS "${tool} -target ${SONIC_ZIG_TARGET}")
	else()
		set(SONIC_ZIG_ARGUMENTS "${tool}")
	endif()
	if(CMAKE_HOST_WIN32)
		set(SONIC_ZIG_SCRIPT ${SONIC_ZIG_TOOLS}/zig-${tool}.cmd)
		file(WRITE ${SONIC_ZIG_SCRIPT} "@\"${ZIG}\" ${SONIC_ZIG_ARGUMENTS} %*\r\n")
	else()
		set(SONIC_ZIG_SCRIPT ${SONIC_ZIG_TOOLS}/zig-${tool})
		file(WRITE ${SONIC_ZIG_SCRIPT} "#!/bin/sh\nexec \"${ZIG}\" ${SONIC_ZIG_ARGUMENTS} \"$@\"\n")
		file(CHMOD ${SONIC_ZIG_SCRIPT} PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE)
	endif()
endforeach()

set(SONIC_ZIG_SUFFIX "")
if(CMAKE_HOST_WIN32)
	set(SONIC_ZIG_SUFFIX ".cmd")
endif()
set(CMAKE_C_COMPILER ${SONIC_ZIG_TOOLS}/zig-cc${SONIC_ZIG_SUFFIX})
set(CMAKE_CXX_COMPILER ${SONIC_ZIG_TOOLS}/zig-c++${SONIC_ZIG_SUFFIX})
set(CMAKE_AR ${SONIC_ZIG_TOOLS}/zig-ar${SONIC_ZIG_SUFFIX} CACHE FILEPATH "" FORCE)
set(CMAKE_RANLIB ${SONIC_ZIG_TOOLS}/zig-ranlib${SONIC_ZIG_SUFFIX} CACHE FILEPATH "" FORCE)
